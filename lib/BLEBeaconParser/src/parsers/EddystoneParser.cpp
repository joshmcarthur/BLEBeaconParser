#include "EddystoneParser.h"

// Eddystone Service UUID (little-endian in advertisement data)
#define EDDYSTONE_SERVICE_UUID_LOW 0xAA
#define EDDYSTONE_SERVICE_UUID_HIGH 0xFE

// Eddystone frame types
#define EDDYSTONE_FRAME_TYPE_UID 0x00
#define EDDYSTONE_FRAME_TYPE_URL 0x10
#define EDDYSTONE_FRAME_TYPE_TLM 0x20

// AD Structure types
#define AD_TYPE_SERVICE_DATA 0x16

// Eddystone-UID frame structure (after frame type byte)
// TX Power (1 byte) + Namespace ID (10 bytes) + Instance ID (6 bytes) = 17 bytes
#define EDDYSTONE_UID_DATA_LENGTH 17

// Eddystone-URL frame structure (after frame type byte)
// TX Power (1 byte) + Encoded URL (variable, max 17 bytes)
#define EDDYSTONE_URL_MIN_DATA_LENGTH 2  // TX Power + at least 1 URL byte

// Eddystone-TLM frame structure (after frame type byte)
// Version (1 byte) + Battery Voltage (2 bytes) + Temperature (2 bytes) +
// Adv Count (4 bytes) + Sec Since Boot (4 bytes) = 13 bytes
#define EDDYSTONE_TLM_DATA_LENGTH 13

bool EddystoneParser::canParse(const uint8_t* data, uint8_t len) {
  const uint8_t* service_data;
  uint8_t service_len;

  return findEddystoneServiceData(data, len, service_data, service_len);
}

bool EddystoneParser::parse(const uint8_t* data, uint8_t len, BeaconData& result) {
  const uint8_t* service_data;
  uint8_t service_len;

  if (!findEddystoneServiceData(data, len, service_data, service_len)) {
    result.valid = false;
    return false;
  }

  // Service data format: [UUID low][UUID high][Frame Type][Frame Data...]
  // We need at least 3 bytes (UUID + Frame Type)
  if (service_len < 3) {
    result.valid = false;
    return false;
  }

  // Frame type is at offset 2 (after UUID)
  uint8_t frame_type = service_data[2];
  const uint8_t* frame_data = &service_data[3];
  uint8_t frame_len = service_len - 3;

  // Route to appropriate parser based on frame type
  switch (frame_type) {
    case EDDYSTONE_FRAME_TYPE_UID:
      return parseUID(frame_data, frame_len, result);

    case EDDYSTONE_FRAME_TYPE_URL:
      return parseURL(frame_data, frame_len, result);

    case EDDYSTONE_FRAME_TYPE_TLM:
      return parseTLM(frame_data, frame_len, result);

    default:
      result.valid = false;
      return false;
  }
}

bool EddystoneParser::findEddystoneServiceData(const uint8_t* data, uint8_t len,
                                               const uint8_t*& out_data, uint8_t& out_len) {
  uint8_t pos = 0;

  // Parse AD structures: [Length][Type][Data...]
  while (pos < len) {
    uint8_t ad_len = data[pos];

    // Check for valid length (0 means end of data)
    if (ad_len == 0) {
      break;
    }

    // Check if we have enough data
    // ad_len includes Type byte, so we need pos + 1 (Length) + ad_len bytes total
    if (pos + 1 + ad_len > len) {
      break;
    }

    uint8_t ad_type = data[pos + 1];

    // Check for Service Data
    if (ad_type == AD_TYPE_SERVICE_DATA) {
      // Service data starts at pos + 2
      // First 2 bytes are Service UUID (little-endian)
      if (ad_len >= 2) {
        uint8_t uuid_low = data[pos + 2];
        uint8_t uuid_high = data[pos + 3];

        // Check for Eddystone Service UUID (0xFEAA)
        if (uuid_low == EDDYSTONE_SERVICE_UUID_LOW && uuid_high == EDDYSTONE_SERVICE_UUID_HIGH) {
          // Service data starts at pos + 2 (includes UUID)
          // ad_len includes Type byte, so Data length is ad_len - 1
          out_data = &data[pos + 2];
          out_len = ad_len - 1;  // Data length (excludes Type byte)
          return true;
        }
      }
    }

    // Move to next AD structure
    pos += ad_len + 1;
  }

  return false;
}

bool EddystoneParser::parseUID(const uint8_t* frame_data, uint8_t frame_len, BeaconData& result) {
  if (frame_len < EDDYSTONE_UID_DATA_LENGTH) {
    result.valid = false;
    return false;
  }

  // TX Power is at offset 0 (signed byte)
  result.eddystone_uid.tx_power = (int8_t)frame_data[0];

  // Namespace ID is at offset 1 (10 bytes)
  for (int i = 0; i < 10; i++) {
    result.eddystone_uid.namespace_id[i] = frame_data[1 + i];
  }

  // Instance ID is at offset 11 (6 bytes)
  for (int i = 0; i < 6; i++) {
    result.eddystone_uid.instance_id[i] = frame_data[11 + i];
  }

  result.type = BEACON_TYPE_EDDYSTONE_UID;
  result.valid = true;
  return true;
}

bool EddystoneParser::parseURL(const uint8_t* frame_data, uint8_t frame_len, BeaconData& result) {
  if (frame_len < EDDYSTONE_URL_MIN_DATA_LENGTH) {
    result.valid = false;
    return false;
  }

  // TX Power is at offset 0 (signed byte)
  result.eddystone_url.tx_power = (int8_t)frame_data[0];

  // URL starts at offset 1
  String url = decodeURLScheme(frame_data[1]);

  if (url.length() == 0) {
    result.valid = false;
    return false;
  }

  // Decode remaining URL bytes
  for (uint8_t i = 2; i < frame_len; i++) {
    String suffix = decodeURLSuffix(frame_data[i]);
    if (suffix.length() > 0) {
      url += suffix;
    } else {
      // If not a special encoding, treat as ASCII character
      url += (char)frame_data[i];
    }
  }

  result.eddystone_url.url = url;
  result.type = BEACON_TYPE_EDDYSTONE_URL;
  result.valid = true;
  return true;
}

bool EddystoneParser::parseTLM(const uint8_t* frame_data, uint8_t frame_len, BeaconData& result) {
  if (frame_len < EDDYSTONE_TLM_DATA_LENGTH) {
    result.valid = false;
    return false;
  }

  // Version is at offset 0 (should be 0x00 for unencrypted TLM)
  uint8_t version = frame_data[0];
  if (version != 0x00) {
    // Encrypted TLM not supported
    result.valid = false;
    return false;
  }

  // Battery Voltage is at offset 1-2 (big-endian, in millivolts)
  result.eddystone_tlm.battery_voltage = (frame_data[1] << 8) | frame_data[2];

  // Temperature is at offset 3-4 (big-endian, signed, 8.8 fixed point)
  int16_t temp_raw = (frame_data[3] << 8) | frame_data[4];
  result.eddystone_tlm.temperature = temp_raw / 256.0f;

  // Advertisement Count is at offset 5-8 (big-endian)
  result.eddystone_tlm.adv_count = ((uint32_t)frame_data[5] << 24) |
                                   ((uint32_t)frame_data[6] << 16) |
                                   ((uint32_t)frame_data[7] << 8) | (uint32_t)frame_data[8];

  // Seconds Since Boot is at offset 9-12 (big-endian, in 0.1 second units)
  uint32_t sec_raw = ((uint32_t)frame_data[9] << 24) | ((uint32_t)frame_data[10] << 16) |
                     ((uint32_t)frame_data[11] << 8) | (uint32_t)frame_data[12];
  result.eddystone_tlm.uptime = sec_raw / 10;  // Convert to seconds

  result.type = BEACON_TYPE_EDDYSTONE_TLM;
  result.valid = true;
  return true;
}

String EddystoneParser::decodeURLScheme(uint8_t encoded_url) {
  // Eddystone URL scheme encoding
  switch (encoded_url) {
    case 0x00:
      return "http://www.";
    case 0x01:
      return "https://www.";
    case 0x02:
      return "http://";
    case 0x03:
      return "https://";
    default:
      return "";
  }
}

String EddystoneParser::decodeURLSuffix(uint8_t encoded_suffix) {
  // Eddystone URL suffix encoding
  switch (encoded_suffix) {
    case 0x00:
      return ".com/";
    case 0x01:
      return ".org/";
    case 0x02:
      return ".edu/";
    case 0x03:
      return ".net/";
    case 0x04:
      return ".info/";
    case 0x05:
      return ".biz/";
    case 0x06:
      return ".gov/";
    case 0x07:
      return ".com";
    case 0x08:
      return ".org";
    case 0x09:
      return ".edu";
    case 0x0A:
      return ".net";
    case 0x0B:
      return ".info";
    case 0x0C:
      return ".biz";
    case 0x0D:
      return ".gov";
    default:
      return "";  // Not a special encoding
  }
}

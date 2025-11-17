#include "iBeaconParser.h"

// Apple Company ID (little-endian in advertisement data)
#define APPLE_COMPANY_ID_LOW 0x4C
#define APPLE_COMPANY_ID_HIGH 0x00

// iBeacon prefix bytes
#define IBEACON_PREFIX_1 0x02
#define IBEACON_PREFIX_2 0x15

// Expected iBeacon data length: prefix (2) + UUID (16) + Major (2) + Minor (2) + TX Power (1) = 23
// bytes
#define IBEACON_DATA_LENGTH 23

// AD Structure types
#define AD_TYPE_MANUFACTURER_SPECIFIC_DATA 0xFF

bool iBeaconParser::canParse(const uint8_t* data, uint8_t len) {
  const uint8_t* mfg_data;
  uint8_t mfg_len;

  if (!findAppleManufacturerData(data, len, mfg_data, mfg_len)) {
    return false;
  }

  // Check if we have enough data for iBeacon prefix
  if (mfg_len < 2) {
    return false;
  }

  // Check for iBeacon prefix (0x02 0x15)
  return (mfg_data[0] == IBEACON_PREFIX_1 && mfg_data[1] == IBEACON_PREFIX_2);
}

bool iBeaconParser::parse(const uint8_t* data, uint8_t len, BeaconData& result) {
  const uint8_t* mfg_data;
  uint8_t mfg_len;

  if (!findAppleManufacturerData(data, len, mfg_data, mfg_len)) {
    result.valid = false;
    return false;
  }

  // Verify iBeacon prefix and length
  if (mfg_len < IBEACON_DATA_LENGTH || mfg_data[0] != IBEACON_PREFIX_1 ||
      mfg_data[1] != IBEACON_PREFIX_2) {
    result.valid = false;
    return false;
  }

  // Extract UUID (16 bytes starting at offset 2)
  result.ibeacon.uuid = uuidToString(&mfg_data[2]);

  // Extract Major (2 bytes, big-endian, at offset 18)
  result.ibeacon.major = (mfg_data[18] << 8) | mfg_data[19];

  // Extract Minor (2 bytes, big-endian, at offset 20)
  result.ibeacon.minor = (mfg_data[20] << 8) | mfg_data[21];

  // Extract TX Power (1 byte, signed, at offset 22)
  result.ibeacon.tx_power = (int8_t)mfg_data[22];

  result.type = BEACON_TYPE_IBEACON;
  result.valid = true;
  return true;
}

String iBeaconParser::uuidToString(const uint8_t* uuid_bytes) {
  String uuid = "";

  // Format: XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX
  for (int i = 0; i < 16; i++) {
    if (uuid.length() > 0) {
      // Add dashes at positions 8, 12, 16, 20
      if (i == 4 || i == 6 || i == 8 || i == 10) {
        uuid += "-";
      }
    }

    // Convert byte to hex
    char hex[3];
    sprintf(hex, "%02X", uuid_bytes[i]);
    uuid += hex;
  }

  return uuid;
}

bool iBeaconParser::findAppleManufacturerData(const uint8_t* data, uint8_t len,
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
    if (pos + ad_len >= len) {
      break;
    }

    uint8_t ad_type = data[pos + 1];

    // Check for Manufacturer Specific Data
    if (ad_type == AD_TYPE_MANUFACTURER_SPECIFIC_DATA) {
      // Manufacturer data starts at pos + 2
      // First 2 bytes are Company ID (little-endian)
      if (ad_len >= 2) {
        uint8_t company_id_low = data[pos + 2];
        uint8_t company_id_high = data[pos + 3];

        // Check for Apple Company ID (0x004C)
        if (company_id_low == APPLE_COMPANY_ID_LOW && company_id_high == APPLE_COMPANY_ID_HIGH) {
          // Manufacturer data starts after Company ID (offset 4)
          out_data = &data[pos + 4];
          out_len = ad_len - 2;  // Subtract Company ID length
          return true;
        }
      }
    }

    // Move to next AD structure
    pos += ad_len + 1;
  }

  return false;
}

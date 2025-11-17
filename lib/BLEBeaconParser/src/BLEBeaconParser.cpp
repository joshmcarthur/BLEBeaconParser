#include "BLEBeaconParser.h"
#include "parsers/AltBeaconParser.h"
#include "parsers/EddystoneParser.h"
#include "parsers/iBeaconParser.h"

// AD Structure types
#define AD_TYPE_MANUFACTURER_SPECIFIC_DATA 0xFF
#define AD_TYPE_SERVICE_DATA 0x16

bool BLEBeaconParser::parse(const uint8_t* data, uint8_t len, BeaconData& result) {
  // Initialize result to unknown/invalid state
  result.type = BEACON_TYPE_UNKNOWN;
  result.valid = false;

  // Validate input
  if (data == nullptr || len == 0) {
    return false;
  }

  // Try each format parser in sequence
  // Order matters: try more specific formats first

  // Try iBeacon
  if (iBeaconParser::canParse(data, len)) {
    if (iBeaconParser::parse(data, len, result)) {
      return true;
    }
  }

  // Try AltBeacon
  if (AltBeaconParser::canParse(data, len)) {
    if (AltBeaconParser::parse(data, len, result)) {
      return true;
    }
  }

  // Try Eddystone (handles UID, URL, and TLM internally)
  if (EddystoneParser::canParse(data, len)) {
    if (EddystoneParser::parse(data, len, result)) {
      return true;
    }
  }

  // No format matched
  return false;
}

bool BLEBeaconParser::findManufacturerData(const uint8_t* data, uint8_t len, uint16_t company_id,
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
        uint16_t found_company_id = (company_id_high << 8) | company_id_low;

        // Check for matching Company ID
        if (found_company_id == company_id) {
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

bool BLEBeaconParser::findServiceData(const uint8_t* data, uint8_t len, uint16_t service_uuid,
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

    // Check for Service Data
    if (ad_type == AD_TYPE_SERVICE_DATA) {
      // Service data starts at pos + 2
      // First 2 bytes are Service UUID (little-endian)
      if (ad_len >= 2) {
        uint8_t uuid_low = data[pos + 2];
        uint8_t uuid_high = data[pos + 3];
        uint16_t found_uuid = (uuid_high << 8) | uuid_low;

        // Check for matching Service UUID
        if (found_uuid == service_uuid) {
          // Service data starts at pos + 2 (includes UUID)
          out_data = &data[pos + 2];
          out_len = ad_len;  // Includes UUID bytes
          return true;
        }
      }
    }

    // Move to next AD structure
    pos += ad_len + 1;
  }

  return false;
}

bool BLEBeaconParser::findADType(const uint8_t* data, uint8_t len, uint8_t ad_type,
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

    uint8_t found_type = data[pos + 1];

    // Check for matching AD type
    if (found_type == ad_type) {
      // AD data starts after type byte (offset 2)
      out_data = &data[pos + 2];
      out_len = ad_len - 1;  // Subtract type byte length
      return true;
    }

    // Move to next AD structure
    pos += ad_len + 1;
  }

  return false;
}

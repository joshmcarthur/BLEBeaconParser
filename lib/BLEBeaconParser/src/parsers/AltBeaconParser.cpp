#include "AltBeaconParser.h"

// Radius Networks Company ID (little-endian in advertisement data)
#define RADIUS_COMPANY_ID_LOW 0x18
#define RADIUS_COMPANY_ID_HIGH 0x01

// AltBeacon beacon code
#define ALTBEACON_CODE_1 0xBE
#define ALTBEACON_CODE_2 0xAC

// Expected AltBeacon data length: beacon code (2) + ID (16) + RSSI (1) +
// Manufacturer Reserved (1) + Major (2) + Minor (2) = 24 bytes
#define ALTBEACON_DATA_LENGTH 24

// AD Structure types
#define AD_TYPE_MANUFACTURER_SPECIFIC_DATA 0xFF

bool AltBeaconParser::canParse(const uint8_t* data, uint8_t len) {
  const uint8_t* mfg_data;
  uint8_t mfg_len;

  if (!findRadiusManufacturerData(data, len, mfg_data, mfg_len)) {
    return false;
  }

  // Check if we have enough data for AltBeacon code
  if (mfg_len < 2) {
    return false;
  }

  // Check for AltBeacon code (0xBE 0xAC)
  return (mfg_data[0] == ALTBEACON_CODE_1 && mfg_data[1] == ALTBEACON_CODE_2);
}

bool AltBeaconParser::parse(const uint8_t* data, uint8_t len, BeaconData& result) {
  const uint8_t* mfg_data;
  uint8_t mfg_len;

  if (!findRadiusManufacturerData(data, len, mfg_data, mfg_len)) {
    result.valid = false;
    return false;
  }

  // Verify AltBeacon code and length
  if (mfg_len < ALTBEACON_DATA_LENGTH || mfg_data[0] != ALTBEACON_CODE_1 ||
      mfg_data[1] != ALTBEACON_CODE_2) {
    result.valid = false;
    return false;
  }

  // Extract Beacon ID (16 bytes starting at offset 2)
  for (int i = 0; i < 16; i++) {
    result.altbeacon.id[i] = mfg_data[2 + i];
  }

  // Extract Reference RSSI (1 byte, signed, at offset 18)
  // Note: This is stored as tx_power in our structure
  result.altbeacon.tx_power = (int8_t)mfg_data[18];

  // Extract Manufacturer Reserved (1 byte, at offset 19)
  result.altbeacon.mfg_reserved = mfg_data[19];

  // Extract Major (2 bytes, big-endian, at offset 20)
  result.altbeacon.major = (mfg_data[20] << 8) | mfg_data[21];

  // Extract Minor (2 bytes, big-endian, at offset 22)
  result.altbeacon.minor = (mfg_data[22] << 8) | mfg_data[23];

  result.type = BEACON_TYPE_ALTBEACON;
  result.valid = true;
  return true;
}

bool AltBeaconParser::findRadiusManufacturerData(const uint8_t* data, uint8_t len,
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

        // Check for Radius Networks Company ID (0x0118)
        if (company_id_low == RADIUS_COMPANY_ID_LOW && company_id_high == RADIUS_COMPANY_ID_HIGH) {
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

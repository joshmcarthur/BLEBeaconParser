#include <unity.h>
#include "BLEBeaconParser.h"
#include "BeaconData.h"

// Sample iBeacon advertisement packet
// Format: [Length][Type=0xFF][CompanyID Low][CompanyID High][0x02][0x15][UUID 16 bytes][Major 2
// bytes][Minor 2 bytes][TX Power] Apple Company ID: 0x004C (little-endian: 0x4C 0x00) iBeacon
// prefix: 0x02 0x15
static const uint8_t ibeacon_packet[] = {
  0x1A,        // Length (26 bytes)
  0xFF,        // Manufacturer Specific Data
  0x4C, 0x00,  // Apple Company ID (little-endian)
  0x02, 0x15,  // iBeacon prefix
  // UUID: 5F2DD896-B886-4549-AE01-E41ACD7A354A
  0x5F, 0x2D, 0xD8, 0x96, 0xB8, 0x86, 0x45, 0x49, 0xAE, 0x01, 0xE4, 0x1A, 0xCD, 0x7A, 0x35, 0x4A,
  0x00, 0x01,  // Major (big-endian: 1)
  0x00, 0x02,  // Minor (big-endian: 2)
  0xC5         // TX Power (-59 dBm)
};

void test_iBeacon_parse() {
  BLEBeaconParser parser;
  BeaconData result;

  bool success = parser.parse(ibeacon_packet, sizeof(ibeacon_packet), result);

  TEST_ASSERT_TRUE(success);
  TEST_ASSERT_TRUE(result.valid);
  TEST_ASSERT_EQUAL(BEACON_TYPE_IBEACON, result.type);
  TEST_ASSERT_EQUAL_STRING("5F2DD896-B886-4549-AE01-E41ACD7A354A",
                           result.getIBeacon().uuid.c_str());
  TEST_ASSERT_EQUAL(1, result.getIBeacon().major);
  TEST_ASSERT_EQUAL(2, result.getIBeacon().minor);
  TEST_ASSERT_EQUAL(-59, result.getIBeacon().tx_power);
}

void test_iBeacon_invalid_data() {
  BLEBeaconParser parser;
  BeaconData result;

  // Test with wrong company ID
  uint8_t wrong_company[] = {0x1A, 0xFF, 0x00, 0x01,  // Wrong company ID
                             0x02, 0x15, 0x5F, 0x2D, 0xD8, 0x96, 0xB8, 0x86, 0x45, 0x49, 0xAE, 0x01,
                             0xE4, 0x1A, 0xCD, 0x7A, 0x35, 0x4A, 0x00, 0x01, 0x00, 0x02, 0xC5};

  bool success = parser.parse(wrong_company, sizeof(wrong_company), result);
  TEST_ASSERT_FALSE(success);
  TEST_ASSERT_FALSE(result.valid);

  // Test with wrong prefix
  uint8_t wrong_prefix[] = {0x1A, 0xFF, 0x4C, 0x00, 0x03, 0x16,  // Wrong prefix
                            0x5F, 0x2D, 0xD8, 0x96, 0xB8, 0x86, 0x45, 0x49, 0xAE, 0x01, 0xE4,
                            0x1A, 0xCD, 0x7A, 0x35, 0x4A, 0x00, 0x01, 0x00, 0x02, 0xC5};

  success = parser.parse(wrong_prefix, sizeof(wrong_prefix), result);
  TEST_ASSERT_FALSE(success);

  // Test with insufficient data
  uint8_t short_data[] = {0x05, 0xFF, 0x4C, 0x00, 0x02};
  success = parser.parse(short_data, sizeof(short_data), result);
  TEST_ASSERT_FALSE(success);
}

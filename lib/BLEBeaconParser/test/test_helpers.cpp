#include <unity.h>
#include "BLEBeaconParser.h"

void test_findManufacturerData() {
  // Test packet with manufacturer data
  uint8_t packet[] = {
    0x05, 0xFF, 0x4C, 0x00, 0x01,  // Apple manufacturer data
    0x03, 0x01, 0x02, 0x03         // Other AD structure
  };

  const uint8_t* out_data;
  uint8_t out_len;

  bool found =
    BLEBeaconParser::findManufacturerData(packet, sizeof(packet), 0x004C, out_data, out_len);

  TEST_ASSERT_TRUE(found);
  TEST_ASSERT_EQUAL(1, out_len);
  TEST_ASSERT_EQUAL(0x01, out_data[0]);
}

void test_findServiceData() {
  // Test packet with service data
  uint8_t packet[] = {
    0x05, 0x16, 0xAA, 0xFE, 0x01,  // Eddystone service data
    0x03, 0x01, 0x02, 0x03         // Other AD structure
  };

  const uint8_t* out_data;
  uint8_t out_len;

  bool found = BLEBeaconParser::findServiceData(packet, sizeof(packet), 0xFEAA, out_data, out_len);

  TEST_ASSERT_TRUE(found);
  TEST_ASSERT_EQUAL(5, out_len);
  TEST_ASSERT_EQUAL(0xAA, out_data[0]);
  TEST_ASSERT_EQUAL(0xFE, out_data[1]);
}

void test_unknown_beacon() {
  BLEBeaconParser parser;
  BeaconData result;

  // Random advertisement data that doesn't match any beacon format
  uint8_t random_data[] = {0x05, 0x01, 0x02, 0x03, 0x04, 0x05};

  bool success = parser.parse(random_data, sizeof(random_data), result);

  TEST_ASSERT_FALSE(success);
  TEST_ASSERT_FALSE(result.valid);
  TEST_ASSERT_EQUAL(BEACON_TYPE_UNKNOWN, result.type);
}

void test_null_data() {
  BLEBeaconParser parser;
  BeaconData result;

  bool success = parser.parse(nullptr, 10, result);

  TEST_ASSERT_FALSE(success);
  TEST_ASSERT_FALSE(result.valid);
}

void test_empty_data() {
  BLEBeaconParser parser;
  BeaconData result;

  uint8_t empty[] = {};
  bool success = parser.parse(empty, 0, result);

  TEST_ASSERT_FALSE(success);
  TEST_ASSERT_FALSE(result.valid);
}

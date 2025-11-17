#include <unity.h>
#include "BLEBeaconParser.h"
#include "BeaconData.h"

// Eddystone-UID packet
// Format: [Length][Type=0x16][UUID Low][UUID High][Frame Type=0x00][TX Power][Namespace 10
// bytes][Instance 6 bytes] Eddystone Service UUID: 0xFEAA (little-endian: 0xAA 0xFE) Length = Type
// (1) + UUID (2) + Frame Type (1) + TX Power (1) + Namespace (10) + Instance (6) = 21
static const uint8_t eddystone_uid_packet[] = {
  0x15,        // Length (21 bytes: Type + 20 bytes data)
  0x16,        // Service Data
  0xAA, 0xFE,  // Eddystone Service UUID (little-endian)
  0x00,        // Frame Type: UID
  0xF0,        // TX Power (-16 dBm)
  // Namespace ID (10 bytes)
  0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99,
  // Instance ID (6 bytes)
  0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};

// Eddystone-URL packet
// Format: [Length][Type=0x16][UUID Low][UUID High][Frame Type=0x10][TX Power][Encoded URL...]
// URL: http://www.example.com/
// Length = Type (1) + UUID (2) + Frame Type (1) + TX Power (1) + Scheme (1) + example (7) + Suffix
// (1) = 14
static const uint8_t eddystone_url_packet[] = {
  0x0E,                                      // Length (14 bytes: Type + 13 bytes data)
  0x16,                                      // Service Data
  0xAA, 0xFE,                                // Eddystone Service UUID (2 bytes)
  0x10,                                      // Frame Type: URL (1 byte)
  0xF0,                                      // TX Power (-16 dBm) (1 byte)
  0x00,                                      // URL Scheme: http://www. (1 byte)
  0x65, 0x78, 0x61, 0x6D, 0x70, 0x6C, 0x65,  // "example" (ASCII, 7 bytes)
  0x00                                       // URL Suffix: .com/ (1 byte)
};

// Eddystone-TLM packet
// Format: [Length][Type=0x16][UUID Low][UUID High][Frame Type=0x20][Version][Battery 2 bytes][Temp
// 2 bytes][Adv Count 4 bytes][Uptime 4 bytes] Length = Type (1) + UUID (2) + Frame Type (1) +
// Version (1) + Battery (2) + Temp (2) + Adv Count (4) + Uptime (4) = 17
static const uint8_t eddystone_tlm_packet[] = {
  0x11,                    // Length (17 bytes: Type + 16 bytes data)
  0x16,                    // Service Data
  0xAA, 0xFE,              // Eddystone Service UUID
  0x20,                    // Frame Type: TLM
  0x00,                    // Version (unencrypted)
  0x0B, 0xB8,              // Battery: 3000 mV (big-endian)
  0x19, 0x00,              // Temperature: 25.0°C (0x1900 = 6400, 6400/256 = 25.0) - 8.8 fixed point
  0x00, 0x00, 0x00, 0x64,  // Adv Count: 100 (big-endian)
  0x00, 0x00, 0x03, 0xE8   // Uptime: 1000 deciseconds = 100 seconds (big-endian)
};

void test_eddystone_uid_parse() {
  BLEBeaconParser parser;
  BeaconData result;

  bool success = parser.parse(eddystone_uid_packet, sizeof(eddystone_uid_packet), result);

  TEST_ASSERT_TRUE(success);
  TEST_ASSERT_TRUE(result.valid);
  TEST_ASSERT_EQUAL(BEACON_TYPE_EDDYSTONE_UID, result.type);

  const EddystoneUIDData& uid = result.getEddystoneUID();
  TEST_ASSERT_EQUAL(-16, uid.tx_power);

  // Check Namespace ID
  uint8_t expected_namespace[] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99};
  TEST_ASSERT_EQUAL_MEMORY(expected_namespace, uid.namespace_id, 10);

  // Check Instance ID
  uint8_t expected_instance[] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
  TEST_ASSERT_EQUAL_MEMORY(expected_instance, uid.instance_id, 6);
}

void test_eddystone_url_parse() {
  BLEBeaconParser parser;
  BeaconData result;

  bool success = parser.parse(eddystone_url_packet, sizeof(eddystone_url_packet), result);

  TEST_ASSERT_TRUE(success);
  TEST_ASSERT_TRUE(result.valid);
  TEST_ASSERT_EQUAL(BEACON_TYPE_EDDYSTONE_URL, result.type);

  const EddystoneURLData& url = result.getEddystoneURL();
  TEST_ASSERT_EQUAL(-16, url.tx_power);
  // URL should be: http://www. + example + .com/
  TEST_ASSERT_EQUAL_STRING("http://www.example.com/", url.url.c_str());
}

void test_eddystone_tlm_parse() {
  BLEBeaconParser parser;
  BeaconData result;

  bool success = parser.parse(eddystone_tlm_packet, sizeof(eddystone_tlm_packet), result);

  TEST_ASSERT_TRUE(success);
  TEST_ASSERT_TRUE(result.valid);
  TEST_ASSERT_EQUAL(BEACON_TYPE_EDDYSTONE_TLM, result.type);

  const EddystoneTLMData& tlm = result.getEddystoneTLM();
  TEST_ASSERT_EQUAL(3000, tlm.battery_voltage);
  TEST_ASSERT_FLOAT_WITHIN(0.1, 25.0, tlm.temperature);
  TEST_ASSERT_EQUAL(100, tlm.adv_count);
  TEST_ASSERT_EQUAL(100, tlm.uptime);
}

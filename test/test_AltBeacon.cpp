#include <unity.h>
#include "BLEBeaconParser.h"
#include "BeaconData.h"

// AltBeacon packet
// Format: [Length][Type=0xFF][CompanyID Low][CompanyID High][0xBE][0xAC][ID 16 bytes][TX Power][Mfg
// Reserved][Major 2 bytes][Minor 2 bytes] Radius Networks Company ID: 0x0118 (little-endian: 0x18
// 0x01) AltBeacon code: 0xBE 0xAC
static const uint8_t altbeacon_packet[] = {
  0x1B,        // Length (27 bytes)
  0xFF,        // Manufacturer Specific Data
  0x18, 0x01,  // Radius Networks Company ID (little-endian)
  0xBE, 0xAC,  // AltBeacon code
  // Beacon ID (16 bytes)
  0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
  0xC5,        // Reference RSSI (-59 dBm)
  0x00,        // Manufacturer Reserved
  0x00, 0x01,  // Major (big-endian: 1)
  0x00, 0x02   // Minor (big-endian: 2)
};

void test_altbeacon_parse() {
  BLEBeaconParser parser;
  BeaconData result;

  bool success = parser.parse(altbeacon_packet, sizeof(altbeacon_packet), result);

  TEST_ASSERT_TRUE(success);
  TEST_ASSERT_TRUE(result.valid);
  TEST_ASSERT_EQUAL(BEACON_TYPE_ALTBEACON, result.type);

  const AltBeaconData& alt = result.getAltBeacon();
  TEST_ASSERT_EQUAL(-59, alt.tx_power);
  TEST_ASSERT_EQUAL(0x00, alt.mfg_reserved);
  TEST_ASSERT_EQUAL(1, alt.major);
  TEST_ASSERT_EQUAL(2, alt.minor);

  // Check Beacon ID
  uint8_t expected_id[] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                           0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
  TEST_ASSERT_EQUAL_MEMORY(expected_id, alt.id, 16);
}

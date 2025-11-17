#include <unity.h>
#include "BLEBeaconParser.h"
#include "BeaconData.h"

void setUp(void) {
  // Set up test fixtures if needed
}

void tearDown(void) {
  // Clean up test fixtures if needed
}

// Forward declarations
void test_iBeacon_parse();
void test_iBeacon_invalid_data();
void test_eddystone_uid_parse();
void test_eddystone_url_parse();
void test_eddystone_tlm_parse();
void test_altbeacon_parse();
void test_findManufacturerData();
void test_findServiceData();
void test_unknown_beacon();
void test_null_data();
void test_empty_data();

int main(int argc, char** argv) {
  UNITY_BEGIN();

  RUN_TEST(test_iBeacon_parse);
  RUN_TEST(test_iBeacon_invalid_data);
  RUN_TEST(test_eddystone_uid_parse);
  RUN_TEST(test_eddystone_url_parse);
  RUN_TEST(test_eddystone_tlm_parse);
  RUN_TEST(test_altbeacon_parse);
  RUN_TEST(test_findManufacturerData);
  RUN_TEST(test_findServiceData);
  RUN_TEST(test_unknown_beacon);
  RUN_TEST(test_null_data);
  RUN_TEST(test_empty_data);

  UNITY_END();
  return 0;
}

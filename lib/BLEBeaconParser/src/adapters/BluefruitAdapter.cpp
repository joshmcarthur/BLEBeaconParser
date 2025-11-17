#include "BluefruitAdapter.h"

// Include Bluefruit headers - this is the only file that needs them
// Users must include bluefruit.h before including this adapter header
// We try to include it here as a convenience, but it may not be available
// on all platforms or if Bluefruit library isn't installed
#if defined(ARDUINO_ARCH_NRF52) || defined(ARDUINO_NRF52_ADAFRUIT)
#include "bluefruit.h"
#else
// For non-nRF52 platforms or when Bluefruit isn't available,
// define a minimal structure to allow compilation
// Note: This adapter is primarily intended for nRF52/Bluefruit
struct ble_gap_evt_adv_report_t {
  uint8_t* data;
  uint8_t dlen;
  int8_t rssi;
  // ... other fields not needed for parsing
};
#endif

bool BluefruitBeaconParser::parse(ble_gap_evt_adv_report_t* report, BeaconData& result) {
  if (report == nullptr) {
    result.valid = false;
    return false;
  }

  // Extract advertisement data from Bluefruit structure
  const uint8_t* adv_data = report->data;
  uint8_t adv_len = report->dlen;

  // Call core parser
  return parser.parse(adv_data, adv_len, result);
}

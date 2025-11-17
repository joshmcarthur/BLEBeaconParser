#ifndef BLUEFRUIT_ADAPTER_H
#define BLUEFRUIT_ADAPTER_H

#include "../BLEBeaconParser.h"
#include "../BeaconData.h"

// Forward declaration to avoid requiring Bluefruit headers in adapter header
// Users must include bluefruit.h before this header
struct ble_gap_evt_adv_report_t;

/**
 * @brief Adapter for Bluefruit library integration
 *
 * Provides a convenient interface for parsing beacons from Bluefruit
 * scan reports. This adapter extracts raw advertisement data from
 * Bluefruit structures and passes it to the core parser.
 *
 * Usage:
 * @code
 * #include "bluefruit.h"
 * #include "BLEBeaconParser/adapters/BluefruitAdapter.h"
 *
 * BluefruitBeaconParser parser;
 *
 * void scan_callback(ble_gap_evt_adv_report_t* report) {
 *   BeaconData result;
 *   if (parser.parse(report, result) && result.valid) {
 *     // Handle parsed beacon
 *   }
 * }
 * @endcode
 */
class BluefruitBeaconParser {
 private:
  BLEBeaconParser parser;

 public:
  /**
   * @brief Parse beacon from Bluefruit scan report
   *
   * Extracts advertisement data from Bluefruit scan report structure
   * and parses it using the core parser.
   *
   * @param report Bluefruit advertisement report structure
   * @param result BeaconData structure to fill with parsed data
   * @return true if a beacon format was successfully parsed, false otherwise
   */
  bool parse(ble_gap_evt_adv_report_t* report, BeaconData& result);
};

#endif  // BLUEFRUIT_ADAPTER_H

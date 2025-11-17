#ifndef BLE_BEACON_PARSER_H
#define BLE_BEACON_PARSER_H

#include <stdint.h>
#include "BeaconData.h"

/**
 * @brief Main BLE Beacon Parser class
 *
 * Provides a unified interface for parsing various BLE beacon formats
 * from raw advertisement packet data. The parser automatically detects
 * the beacon format and returns a unified BeaconData structure.
 *
 * Supported formats:
 * - iBeacon (Apple)
 * - Eddystone (Google) - UID, URL, and TLM frames
 * - AltBeacon (Radius Networks)
 *
 * Usage:
 * @code
 * BLEBeaconParser parser;
 * BeaconData result;
 *
 * if (parser.parse(adv_data, adv_len, result) && result.valid) {
 *   switch (result.type) {
 *     case BEACON_TYPE_IBEACON:
 *       Serial.println(result.getIBeacon().uuid);
 *       break;
 *     // ... handle other types
 *   }
 * }
 * @endcode
 */
class BLEBeaconParser {
 public:
  /**
   * @brief Parse beacon data from raw advertisement packet
   *
   * Automatically detects the beacon format and parses it into a unified
   * BeaconData structure. Tries each format parser in sequence until one
   * successfully parses the data.
   *
   * @param data Raw advertisement packet data
   * @param len Length of advertisement data
   * @param result BeaconData structure to fill with parsed data
   * @return true if a beacon format was successfully parsed, false otherwise
   */
  bool parse(const uint8_t* data, uint8_t len, BeaconData& result);

  /**
   * @brief Find manufacturer-specific data by company ID
   *
   * Parses AD structures to find manufacturer-specific data (AD Type 0xFF)
   * matching the specified company ID.
   *
   * @param data Raw advertisement data
   * @param len Length of advertisement data
   * @param company_id Company ID to search for (little-endian format)
   * @param out_data Output pointer to manufacturer data (after company ID)
   * @param out_len Output length of manufacturer data
   * @return true if manufacturer data was found
   */
  static bool findManufacturerData(const uint8_t* data, uint8_t len, uint16_t company_id,
                                   const uint8_t*& out_data, uint8_t& out_len);

  /**
   * @brief Find service data by service UUID
   *
   * Parses AD structures to find service data (AD Type 0x16) matching
   * the specified service UUID.
   *
   * @param data Raw advertisement data
   * @param len Length of advertisement data
   * @param service_uuid Service UUID to search for (little-endian format)
   * @param out_data Output pointer to service data (including UUID)
   * @param out_len Output length of service data
   * @return true if service data was found
   */
  static bool findServiceData(const uint8_t* data, uint8_t len, uint16_t service_uuid,
                              const uint8_t*& out_data, uint8_t& out_len);

 private:
  /**
   * @brief Parse AD structure to find specific AD type
   *
   * Helper function to parse BLE AD structures and find data of a specific type.
   *
   * @param data Raw advertisement data
   * @param len Length of advertisement data
   * @param ad_type AD type to search for
   * @param out_data Output pointer to AD data (after type byte)
   * @param out_len Output length of AD data
   * @return true if AD type was found
   */
  static bool findADType(const uint8_t* data, uint8_t len, uint8_t ad_type,
                         const uint8_t*& out_data, uint8_t& out_len);
};

#endif  // BLE_BEACON_PARSER_H

#ifndef IBEACON_PARSER_H
#define IBEACON_PARSER_H

#include <stdint.h>
#include "../BeaconData.h"

/**
 * @brief Parser for iBeacon format (Apple)
 *
 * iBeacon format:
 * - Manufacturer Specific Data (AD Type 0xFF)
 * - Apple Company ID: 0x004C (little-endian: 0x4C 0x00)
 * - iBeacon prefix: 0x02 0x15
 * - UUID: 16 bytes
 * - Major: 2 bytes (big-endian)
 * - Minor: 2 bytes (big-endian)
 * - TX Power: 1 byte (signed)
 */
class iBeaconParser {
 public:
  /**
   * @brief Check if the advertisement data matches iBeacon format
   * @param data Raw advertisement data
   * @param len Length of advertisement data
   * @return true if data matches iBeacon format
   */
  static bool canParse(const uint8_t* data, uint8_t len);

  /**
   * @brief Parse iBeacon data from advertisement packet
   * @param data Raw advertisement data
   * @param len Length of advertisement data
   * @param result BeaconData structure to fill with parsed data
   * @return true if parsing was successful
   */
  static bool parse(const uint8_t* data, uint8_t len, BeaconData& result);

 private:
  /**
   * @brief Convert UUID bytes to hex string with dashes
   * @param uuid_bytes 16-byte UUID array
   * @return UUID string in format "XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX"
   */
  static String uuidToString(const uint8_t* uuid_bytes);

  /**
   * @brief Find manufacturer-specific data with Apple Company ID
   * @param data Raw advertisement data
   * @param len Length of advertisement data
   * @param out_data Output pointer to manufacturer data
   * @param out_len Output length of manufacturer data
   * @return true if found
   */
  static bool findAppleManufacturerData(const uint8_t* data, uint8_t len, const uint8_t*& out_data,
                                        uint8_t& out_len);
};

#endif  // IBEACON_PARSER_H

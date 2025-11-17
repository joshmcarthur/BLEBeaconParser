#ifndef ALTBEACON_PARSER_H
#define ALTBEACON_PARSER_H

#include <stdint.h>
#include "../BeaconData.h"

/**
 * @brief Parser for AltBeacon format (Radius Networks)
 *
 * AltBeacon format:
 * - Manufacturer Specific Data (AD Type 0xFF)
 * - Radius Networks Company ID: 0x0118 (little-endian: 0x18 0x01)
 * - AltBeacon structure:
 *   - Beacon Code: 0xBE 0xAC (2 bytes)
 *   - Beacon ID: 16 bytes
 *   - Reference RSSI: 1 byte (signed)
 *   - Manufacturer Reserved: 1 byte
 *   - Major: 2 bytes (big-endian)
 *   - Minor: 2 bytes (big-endian)
 */
class AltBeaconParser {
 public:
  /**
   * @brief Check if the advertisement data matches AltBeacon format
   * @param data Raw advertisement data
   * @param len Length of advertisement data
   * @return true if data matches AltBeacon format
   */
  static bool canParse(const uint8_t* data, uint8_t len);

  /**
   * @brief Parse AltBeacon data from advertisement packet
   * @param data Raw advertisement data
   * @param len Length of advertisement data
   * @param result BeaconData structure to fill with parsed data
   * @return true if parsing was successful
   */
  static bool parse(const uint8_t* data, uint8_t len, BeaconData& result);

 private:
  /**
   * @brief Find manufacturer-specific data with Radius Networks Company ID
   * @param data Raw advertisement data
   * @param len Length of advertisement data
   * @param out_data Output pointer to manufacturer data
   * @param out_len Output length of manufacturer data
   * @return true if found
   */
  static bool findRadiusManufacturerData(const uint8_t* data, uint8_t len, const uint8_t*& out_data,
                                         uint8_t& out_len);
};

#endif  // ALTBEACON_PARSER_H

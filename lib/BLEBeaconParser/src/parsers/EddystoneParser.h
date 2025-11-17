#ifndef EDDYSTONE_PARSER_H
#define EDDYSTONE_PARSER_H

#include <stdint.h>
#include "../BeaconData.h"

/**
 * @brief Parser for Eddystone format (Google)
 *
 * Eddystone format:
 * - Service Data (AD Type 0x16) with Eddystone Service UUID (0xFEAA)
 * - Frame type byte determines the frame format:
 *   - 0x00: Eddystone-UID
 *   - 0x10: Eddystone-URL
 *   - 0x20: Eddystone-TLM
 */
class EddystoneParser {
 public:
  /**
   * @brief Check if the advertisement data matches Eddystone format
   * @param data Raw advertisement data
   * @param len Length of advertisement data
   * @return true if data matches Eddystone format
   */
  static bool canParse(const uint8_t* data, uint8_t len);

  /**
   * @brief Parse Eddystone data from advertisement packet
   * @param data Raw advertisement data
   * @param len Length of advertisement data
   * @param result BeaconData structure to fill with parsed data
   * @return true if parsing was successful
   */
  static bool parse(const uint8_t* data, uint8_t len, BeaconData& result);

 private:
  /**
   * @brief Find service data with Eddystone Service UUID
   * @param data Raw advertisement data
   * @param len Length of advertisement data
   * @param out_data Output pointer to service data
   * @param out_len Output length of service data
   * @return true if found
   */
  static bool findEddystoneServiceData(const uint8_t* data, uint8_t len, const uint8_t*& out_data,
                                       uint8_t& out_len);

  /**
   * @brief Parse Eddystone-UID frame
   * @param frame_data Frame data (after frame type byte)
   * @param frame_len Frame data length
   * @param result BeaconData structure to fill
   * @return true if parsing was successful
   */
  static bool parseUID(const uint8_t* frame_data, uint8_t frame_len, BeaconData& result);

  /**
   * @brief Parse Eddystone-URL frame
   * @param frame_data Frame data (after frame type byte)
   * @param frame_len Frame data length
   * @param result BeaconData structure to fill
   * @return true if parsing was successful
   */
  static bool parseURL(const uint8_t* frame_data, uint8_t frame_len, BeaconData& result);

  /**
   * @brief Parse Eddystone-TLM frame
   * @param frame_data Frame data (after frame type byte)
   * @param frame_len Frame data length
   * @param result BeaconData structure to fill
   * @return true if parsing was successful
   */
  static bool parseTLM(const uint8_t* frame_data, uint8_t frame_len, BeaconData& result);

  /**
   * @brief Decode Eddystone URL encoding
   * @param encoded_url Encoded URL byte
   * @return Decoded URL string or empty string if invalid
   */
  static String decodeURLScheme(uint8_t encoded_url);

  /**
   * @brief Decode Eddystone URL suffix
   * @param encoded_suffix Encoded suffix byte
   * @return Decoded URL suffix string or empty string if invalid
   */
  static String decodeURLSuffix(uint8_t encoded_suffix);
};

#endif  // EDDYSTONE_PARSER_H

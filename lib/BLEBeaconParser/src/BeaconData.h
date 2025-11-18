#ifndef BEACON_DATA_H
#define BEACON_DATA_H

#include <stdint.h>
#include <new>
#ifdef NATIVE_BUILD
#include "mock_arduino.h"
#else
#include <Arduino.h>
#endif

/**
 * @brief Enumeration of supported beacon types
 */
enum BeaconType {
  BEACON_TYPE_UNKNOWN = 0,
  BEACON_TYPE_IBEACON,
  BEACON_TYPE_EDDYSTONE_UID,
  BEACON_TYPE_EDDYSTONE_URL,
  BEACON_TYPE_EDDYSTONE_TLM,
  BEACON_TYPE_ALTBEACON
};

/**
 * @brief iBeacon data structure
 */
struct iBeaconData {
  String uuid;  // UUID as hex string (e.g., "5F2DD896-B886-4549-AE01-E41ACD7A354A")
  uint16_t major;
  uint16_t minor;
  int8_t tx_power;
};

/**
 * @brief Eddystone UID data structure
 */
struct EddystoneUIDData {
  uint8_t namespace_id[10];
  uint8_t instance_id[6];
  int8_t tx_power;
};

/**
 * @brief Eddystone URL data structure
 */
struct EddystoneURLData {
  String url;
  int8_t tx_power;
};

/**
 * @brief Eddystone TLM (Telemetry) data structure
 */
struct EddystoneTLMData {
  uint16_t battery_voltage;  // Battery voltage in millivolts
  float temperature;         // Temperature in degrees Celsius
  uint32_t adv_count;        // Advertisement count
  uint32_t uptime;           // Uptime in seconds
};

/**
 * @brief AltBeacon data structure
 */
struct AltBeaconData {
  uint8_t id[16];  // Beacon ID (16 bytes)
  uint16_t major;
  uint16_t minor;
  int8_t tx_power;
  uint8_t mfg_reserved;  // Manufacturer reserved byte
};

/**
 * @brief Unified beacon data structure containing all format-specific data
 *
 * Use the type field to determine which union member contains valid data.
 * Always check the valid flag before accessing union members.
 */
struct BeaconData {
  BeaconType type;
  bool valid;

  union {
    iBeaconData ibeacon;
    EddystoneUIDData eddystone_uid;
    EddystoneURLData eddystone_url;
    EddystoneTLMData eddystone_tlm;
    AltBeaconData altbeacon;
  };

  /**
   * @brief Constructor - initializes to unknown/invalid state
   */
  BeaconData() : type(BEACON_TYPE_UNKNOWN), valid(false) {
    // Initialize union members using placement new
    new (&ibeacon) iBeaconData();
  }

  /**
   * @brief Destructor - properly destroys union members
   */
  ~BeaconData() {
    // Call destructor for the active union member
    switch (type) {
      case BEACON_TYPE_IBEACON:
        ibeacon.~iBeaconData();
        break;
      case BEACON_TYPE_EDDYSTONE_URL:
        eddystone_url.~EddystoneURLData();
        break;
      default:
        // Other types don't have non-trivial destructors
        break;
    }
  }

  /**
   * @brief Copy constructor
   */
  BeaconData(const BeaconData& other) : type(other.type), valid(other.valid) {
    // Copy the appropriate union member
    switch (type) {
      case BEACON_TYPE_IBEACON:
        new (&ibeacon) iBeaconData(other.ibeacon);
        break;
      case BEACON_TYPE_EDDYSTONE_UID:
        eddystone_uid = other.eddystone_uid;
        break;
      case BEACON_TYPE_EDDYSTONE_URL:
        new (&eddystone_url) EddystoneURLData(other.eddystone_url);
        break;
      case BEACON_TYPE_EDDYSTONE_TLM:
        eddystone_tlm = other.eddystone_tlm;
        break;
      case BEACON_TYPE_ALTBEACON:
        altbeacon = other.altbeacon;
        break;
      default:
        break;
    }
  }

  /**
   * @brief Assignment operator
   */
  BeaconData& operator=(const BeaconData& other) {
    if (this != &other) {
      // Destroy current union member
      switch (type) {
        case BEACON_TYPE_IBEACON:
          ibeacon.~iBeaconData();
          break;
        case BEACON_TYPE_EDDYSTONE_URL:
          eddystone_url.~EddystoneURLData();
          break;
        default:
          break;
      }

      type = other.type;
      valid = other.valid;

      // Copy the appropriate union member
      switch (type) {
        case BEACON_TYPE_IBEACON:
          new (&ibeacon) iBeaconData(other.ibeacon);
          break;
        case BEACON_TYPE_EDDYSTONE_UID:
          eddystone_uid = other.eddystone_uid;
          break;
        case BEACON_TYPE_EDDYSTONE_URL:
          new (&eddystone_url) EddystoneURLData(other.eddystone_url);
          break;
        case BEACON_TYPE_EDDYSTONE_TLM:
          eddystone_tlm = other.eddystone_tlm;
          break;
        case BEACON_TYPE_ALTBEACON:
          altbeacon = other.altbeacon;
          break;
        default:
          break;
      }
    }
    return *this;
  }

  /**
   * @brief Get iBeacon data (only valid if type == BEACON_TYPE_IBEACON)
   */
  const iBeaconData& getIBeacon() const {
    return ibeacon;
  }

  /**
   * @brief Get Eddystone UID data (only valid if type == BEACON_TYPE_EDDYSTONE_UID)
   */
  const EddystoneUIDData& getEddystoneUID() const {
    return eddystone_uid;
  }

  /**
   * @brief Get Eddystone URL data (only valid if type == BEACON_TYPE_EDDYSTONE_URL)
   */
  const EddystoneURLData& getEddystoneURL() const {
    return eddystone_url;
  }

  /**
   * @brief Get Eddystone TLM data (only valid if type == BEACON_TYPE_EDDYSTONE_TLM)
   */
  const EddystoneTLMData& getEddystoneTLM() const {
    return eddystone_tlm;
  }

  /**
   * @brief Get AltBeacon data (only valid if type == BEACON_TYPE_ALTBEACON)
   */
  const AltBeaconData& getAltBeacon() const {
    return altbeacon;
  }
};

#endif  // BEACON_DATA_H

/**
 * @file generic_example/main.cpp
 * @brief Example demonstrating generic BLE beacon parsing with raw byte arrays
 *
 * This example shows how to use the BLEBeaconParser library with raw
 * advertisement packet data, without any BLE library dependencies.
 */

#include <Arduino.h>
#include "BLEBeaconParser.h"

BLEBeaconParser parser;

// Example iBeacon advertisement data
// Format: [Length][Type][Company ID Low][Company ID High][iBeacon Prefix][UUID 16
// bytes][Major][Minor][TX Power]
uint8_t ibeacon_data[] = {
  0x1A,        // Length (26 bytes)
  0xFF,        // Type: Manufacturer Specific Data
  0x4C, 0x00,  // Apple Company ID (little-endian)
  0x02, 0x15,  // iBeacon prefix
  // UUID: 5F2DD896-B886-4549-AE01-E41ACD7A354A
  0x5F, 0x2D, 0xD8, 0x96, 0xB8, 0x86, 0x45, 0x49, 0xAE, 0x01, 0xE4, 0x1A, 0xCD, 0x7A, 0x35, 0x4A,
  0x00, 0x01,  // Major: 1
  0x00, 0x02,  // Minor: 2
  0xC5         // TX Power: -59 dBm
};

// Example Eddystone-UID advertisement data
// Format: [Length][Type][UUID Low][UUID High][Frame Type][TX Power][Namespace 10 bytes][Instance 6
// bytes]
uint8_t eddystone_uid_data[] = {0x15,        // Length (21 bytes)
                                0x16,        // Type: Service Data
                                0xAA, 0xFE,  // Eddystone Service UUID (little-endian)
                                0x00,        // Frame Type: UID
                                0xEE,        // TX Power: -18 dBm
                                // Namespace ID: 00000000000000000000
                                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                // Instance ID: 000000000000
                                0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// Example Eddystone-URL advertisement data
// Format: [Length][Type][UUID Low][UUID High][Frame Type][TX Power][Encoded URL...]
uint8_t eddystone_url_data[] = {
  0x0F,        // Length (15 bytes)
  0x16,        // Type: Service Data
  0xAA, 0xFE,  // Eddystone Service UUID (little-endian)
  0x10,        // Frame Type: URL
  0xF4,        // TX Power: -12 dBm
  0x02,        // URL Scheme: http://
  0x67, 0x6F, 0x6F, 0x67,
  0x6C, 0x65, 0x07  // "google.com"
  // 0x67='g', 0x6F='o', 0x6F='o', 0x67='g', 0x6C='l', 0x65='e', 0x07='.com'
};

// Example AltBeacon advertisement data
// Format: [Length][Type][Company ID Low][Company ID High][Beacon Code][ID 16
// bytes][RSSI][Reserved][Major][Minor]
uint8_t altbeacon_data[] = {
  0x1C,        // Length (28 bytes)
  0xFF,        // Type: Manufacturer Specific Data
  0x18, 0x01,  // Radius Networks Company ID (little-endian)
  0xBE, 0xAC,  // AltBeacon code
  // Beacon ID: 00000000000000000000000000000000
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0xC5,        // Reference RSSI: -59 dBm
  0x00,        // Manufacturer Reserved
  0x00, 0x01,  // Major: 1
  0x00, 0x02   // Minor: 2
};

void printBeaconData(const BeaconData& result) {
  Serial.print("Beacon Type: ");

  switch (result.type) {
    case BEACON_TYPE_IBEACON:
      Serial.println("iBeacon");
      Serial.print("  UUID: ");
      Serial.println(result.getIBeacon().uuid);
      Serial.print("  Major: ");
      Serial.println(result.getIBeacon().major);
      Serial.print("  Minor: ");
      Serial.println(result.getIBeacon().minor);
      Serial.print("  TX Power: ");
      Serial.print(result.getIBeacon().tx_power);
      Serial.println(" dBm");
      break;

    case BEACON_TYPE_EDDYSTONE_UID:
      Serial.println("Eddystone-UID");
      Serial.print("  Namespace ID: ");
      for (int i = 0; i < 10; i++) {
        if (result.getEddystoneUID().namespace_id[i] < 0x10)
          Serial.print("0");
        Serial.print(result.getEddystoneUID().namespace_id[i], HEX);
      }
      Serial.println();
      Serial.print("  Instance ID: ");
      for (int i = 0; i < 6; i++) {
        if (result.getEddystoneUID().instance_id[i] < 0x10)
          Serial.print("0");
        Serial.print(result.getEddystoneUID().instance_id[i], HEX);
      }
      Serial.println();
      Serial.print("  TX Power: ");
      Serial.print(result.getEddystoneUID().tx_power);
      Serial.println(" dBm");
      break;

    case BEACON_TYPE_EDDYSTONE_URL:
      Serial.println("Eddystone-URL");
      Serial.print("  URL: ");
      Serial.println(result.getEddystoneURL().url);
      Serial.print("  TX Power: ");
      Serial.print(result.getEddystoneURL().tx_power);
      Serial.println(" dBm");
      break;

    case BEACON_TYPE_EDDYSTONE_TLM:
      Serial.println("Eddystone-TLM");
      Serial.print("  Battery Voltage: ");
      Serial.print(result.getEddystoneTLM().battery_voltage);
      Serial.println(" mV");
      Serial.print("  Temperature: ");
      Serial.print(result.getEddystoneTLM().temperature);
      Serial.println(" °C");
      Serial.print("  Advertisement Count: ");
      Serial.println(result.getEddystoneTLM().adv_count);
      Serial.print("  Uptime: ");
      Serial.print(result.getEddystoneTLM().uptime);
      Serial.println(" seconds");
      break;

    case BEACON_TYPE_ALTBEACON:
      Serial.println("AltBeacon");
      Serial.print("  Beacon ID: ");
      for (int i = 0; i < 16; i++) {
        if (result.getAltBeacon().id[i] < 0x10)
          Serial.print("0");
        Serial.print(result.getAltBeacon().id[i], HEX);
      }
      Serial.println();
      Serial.print("  Major: ");
      Serial.println(result.getAltBeacon().major);
      Serial.print("  Minor: ");
      Serial.println(result.getAltBeacon().minor);
      Serial.print("  TX Power: ");
      Serial.print(result.getAltBeacon().tx_power);
      Serial.println(" dBm");
      Serial.print("  Manufacturer Reserved: 0x");
      Serial.println(result.getAltBeacon().mfg_reserved, HEX);
      break;

    default:
      Serial.println("Unknown");
      break;
  }

  Serial.println();
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  Serial.println("BLE Beacon Parser - Generic Example");
  Serial.println("===================================");
  Serial.println();

  BeaconData result;

  // Parse iBeacon
  Serial.println("Parsing iBeacon data...");
  if (parser.parse(ibeacon_data, sizeof(ibeacon_data), result) && result.valid) {
    printBeaconData(result);
  } else {
    Serial.println("Failed to parse iBeacon data");
    Serial.println();
  }

  // Parse Eddystone-UID
  Serial.println("Parsing Eddystone-UID data...");
  if (parser.parse(eddystone_uid_data, sizeof(eddystone_uid_data), result) && result.valid) {
    printBeaconData(result);
  } else {
    Serial.println("Failed to parse Eddystone-UID data");
    Serial.println();
  }

  // Parse Eddystone-URL
  Serial.println("Parsing Eddystone-URL data...");
  if (parser.parse(eddystone_url_data, sizeof(eddystone_url_data), result) && result.valid) {
    printBeaconData(result);
  } else {
    Serial.println("Failed to parse Eddystone-URL data");
    Serial.println();
  }

  // Parse AltBeacon
  Serial.println("Parsing AltBeacon data...");
  if (parser.parse(altbeacon_data, sizeof(altbeacon_data), result) && result.valid) {
    printBeaconData(result);
  } else {
    Serial.println("Failed to parse AltBeacon data");
    Serial.println();
  }

  Serial.println("Example complete!");
}

void loop() {
  // Nothing to do in loop
  delay(1000);
}

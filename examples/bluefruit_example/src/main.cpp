/**
 * @file bluefruit_example/main.cpp
 * @brief Example demonstrating BLE beacon parsing with Bluefruit library
 *
 * This example shows how to use the BLEBeaconParser library with the
 * Bluefruit library on nRF52 boards. It scans for BLE advertisements
 * and parses any detected beacons.
 *
 * Hardware Requirements:
 * - Adafruit nRF52 Feather or compatible board
 * - Bluefruit library installed
 */

#include <Arduino.h>
#include "BLEBeaconParser/adapters/BluefruitAdapter.h"
#include "bluefruit.h"

BluefruitBeaconParser parser;

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

/**
 * @brief Scan callback function called when an advertisement is received
 */
void scan_callback(ble_gap_evt_adv_report_t* report) {
  BeaconData result;

  // Try to parse the advertisement as a beacon
  if (parser.parse(report, result) && result.valid) {
    Serial.println("=== Beacon Detected ===");
    Serial.print("RSSI: ");
    Serial.print(report->rssi);
    Serial.println(" dBm");
    printBeaconData(result);
  }

  // Resume scanning
  Bluefruit.Scanner.resume();
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  Serial.println("BLE Beacon Parser - Bluefruit Example");
  Serial.println("======================================");
  Serial.println();

  // Initialize Bluefruit
  Bluefruit.begin();
  Bluefruit.setTxPower(4);  // Set TX power to +4 dBm

  // Initialize BLE Scanner
  Bluefruit.Scanner.setRxCallback(scan_callback);
  Bluefruit.Scanner.restartOnDisconnect(true);
  Bluefruit.Scanner.setInterval(160, 80);  // in units of 0.625 ms
  Bluefruit.Scanner.useActiveScan(false);  // Don't use active scan
  Bluefruit.Scanner.start(0);              // 0 = Don't stop scanning after n seconds

  Serial.println("Scanning for beacons...");
  Serial.println();
}

void loop() {
  // Scanning is handled in the callback
  delay(100);
}


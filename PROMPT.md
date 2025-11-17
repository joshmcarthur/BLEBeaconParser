# Create a BLE Beacon Parser Library

Create a standalone, library-agnostic BLE beacon parser library for Arduino/PlatformIO that parses iBeacon, Eddystone, and AltBeacon formats from raw advertisement packet data.

## Requirements

### Core Design Principles
1. Library-agnostic: core parser works with raw byte arrays (`uint8_t* data, uint8_t len`), no BLE library dependencies
2. Portable: usable with Bluefruit (nRF52), ArduinoBLE, ESP32 BLE, etc.
3. Lightweight: minimal memory footprint, suitable for embedded systems
4. Extensible: easy to add new beacon formats

### Supported Beacon Formats

#### 1. iBeacon (Apple)
- Format: Manufacturer Specific Data (0xFF) with Apple Company ID (0x004C)
- Structure: `[0x02][0x15][UUID 16 bytes][Major 2 bytes][Minor 2 bytes][TX Power 1 byte]`
- Extract: UUID (16 bytes as hex string), Major (uint16_t), Minor (uint16_t), TX Power (int8_t)

#### 2. Eddystone (Google)
- Eddystone-UID: Namespace (10 bytes) + Instance (6 bytes)
- Eddystone-URL: Encoded URL string
- Eddystone-TLM: Telemetry data
- Extract: Type, Namespace/URL/TLM data as appropriate

#### 3. AltBeacon (Radius Networks)
- Format: Manufacturer Specific Data with Radius Networks Company ID
- Structure: Similar to iBeacon but with different layout
- Extract: ID (16 bytes), Major, Minor, TX Power

### Library Structure

```
BLEBeaconParser/
├── src/
│   ├── BLEBeaconParser.h          # Main header with parser class
│   ├── BLEBeaconParser.cpp        # Core parsing implementation
│   ├── BeaconTypes.h              # Data structures for parsed beacons
│   └── adapters/
│       ├── BluefruitAdapter.h     # Thin wrapper for Bluefruit library
│       └── BluefruitAdapter.cpp
├── examples/
│   ├── bluefruit_example/
│   │   └── main.cpp               # Example using Bluefruit
│   └── generic_example/
│       └── main.cpp               # Example with raw bytes
├── library.properties             # Arduino library metadata
└── README.md                      # Documentation
```

### API Design

#### Core Parser Class (`BLEBeaconParser.h`)
```cpp
class BLEBeaconParser {
public:
  // iBeacon data structure
  struct iBeaconData {
    String uuid;        // UUID as hex string (e.g., "5F2DD896-B886-4549-AE01-E41ACD7A354A")
    uint16_t major;
    uint16_t minor;
    int8_t tx_power;
    bool valid;
  };

  // Eddystone data structures
  struct EddystoneUID {
    uint8_t namespace_id[10];
    uint8_t instance_id[6];
    int8_t tx_power;
    bool valid;
  };

  struct EddystoneURL {
    String url;
    int8_t tx_power;
    bool valid;
  };

  // Parsing methods (work with raw bytes)
  bool parse_iBeacon(const uint8_t* data, uint8_t len, iBeaconData& result);
  bool parse_Eddystone(const uint8_t* data, uint8_t len, EddystoneUID& uid_result, EddystoneURL& url_result);
  bool parse_AltBeacon(const uint8_t* data, uint8_t len, AltBeaconData& result);

  // Helper: Parse AD structure to find Manufacturer Specific Data
  bool findManufacturerData(const uint8_t* data, uint8_t len, uint16_t company_id, uint8_t*& out_data, uint8_t& out_len);
};
```

#### Bluefruit Adapter (`adapters/BluefruitAdapter.h`)
```cpp
#include "BLEBeaconParser.h"
#include "bluefruit.h"  // Bluefruit-specific include

class BluefruitBeaconParser {
private:
  BLEBeaconParser parser;

public:
  // Convenience method for Bluefruit scan callback
  bool parse(ble_gap_evt_adv_report_t* report, BLEBeaconParser::iBeaconData& ibeacon);
  bool parse(ble_gap_evt_adv_report_t* report, BLEBeaconParser::EddystoneUID& eddystone);
  // ... other parse methods
};
```

### Implementation Details

1. AD Structure Parsing:
   - Parse BLE AD structure: `[Length][Type][Data]`
   - Handle multiple AD elements in one packet
   - Find Manufacturer Specific Data (Type 0xFF)

2. iBeacon Parsing:
   - Check for Apple Company ID (0x004C, little-endian: 0x4C 0x00)
   - Verify iBeacon prefix (0x02 0x15)
   - Extract UUID (16 bytes, convert to hex string with dashes)
   - Extract Major/Minor (big-endian)
   - Extract TX Power (signed byte)

3. Eddystone Parsing:
   - Check for Eddystone Service UUID in Service Data (0x16) or Manufacturer Data
   - Parse frame type byte
   - Extract data based on frame type (UID, URL, TLM)

4. Error Handling:
   - Return `false` if parsing fails
   - Set `valid = false` in result structures on failure
   - Validate data lengths before parsing

### Example Usage

#### Generic (raw bytes):
```cpp
#include "BLEBeaconParser.h"

BLEBeaconParser parser;
BLEBeaconParser::iBeaconData ibeacon;

uint8_t adv_data[] = { /* raw advertisement data */ };
uint8_t adv_len = sizeof(adv_data);

if (parser.parse_iBeacon(adv_data, adv_len, ibeacon) && ibeacon.valid) {
  Serial.print("UUID: "); Serial.println(ibeacon.uuid);
  Serial.print("Major: "); Serial.println(ibeacon.major);
  Serial.print("Minor: "); Serial.println(ibeacon.minor);
}
```

#### Bluefruit:
```cpp
#include "adapters/BluefruitAdapter.h"

BluefruitBeaconParser parser;

void scan_callback(ble_gap_evt_adv_report_t* report) {
  BLEBeaconParser::iBeaconData ibeacon;
  if (parser.parse(report, ibeacon) && ibeacon.valid) {
    // Use parsed data
  }
}
```

### Additional Requirements

1. Memory efficiency:
   - Avoid dynamic allocation where possible
   - Use String class sparingly (consider char arrays for UUID)
   - Minimize stack usage

2. Documentation:
   - Inline comments explaining beacon formats
   - README with usage examples
   - Doxygen-style comments for public API

3. Testing considerations:
   - Design should allow unit testing with raw byte arrays
   - Handle edge cases (malformed packets, wrong lengths, etc.)

4. Platform compatibility:
   - Works with Arduino String class
   - Compatible with Arduino/PlatformIO build systems
   - No C++17+ features (use C++11 compatible code)

### Deliverables

1. Complete library code with all files
2. Example code for Bluefruit integration
3. Example code for generic/raw byte usage
4. README.md with:
   - Overview and supported formats
   - Installation instructions
   - Usage examples
   - API reference
   - How to add adapters for other libraries

Create a production-ready library that can be easily integrated into the outdoor-tracker project and reused across other BLE projects.
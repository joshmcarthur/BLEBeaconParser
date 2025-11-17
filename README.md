# BLE Beacon Parser Library

A library-agnostic BLE beacon parser for Arduino/PlatformIO that supports iBeacon, Eddystone, and AltBeacon formats.

## Features

- **Multi-format support**: iBeacon (Apple), Eddystone (Google), and AltBeacon (Radius Networks)
- **Library-agnostic**: Works with any BLE library by parsing raw advertisement data
- **Unified API**: Single interface for all beacon formats
- **Well-tested**: Comprehensive unit test suite
- **CI/CD**: Automated testing and linting via GitHub Actions

## Installation

### PlatformIO

Add to your `platformio.ini`:

```ini
lib_deps =
    https://github.com/joshmcarthur/BLEBeaconParser.git
```

### Arduino IDE

1. Download the library as a ZIP file
2. Install via Sketch → Include Library → Add .ZIP Library

## Usage

### Basic Example

```cpp
#include "BLEBeaconParser.h"

BLEBeaconParser parser;
BeaconData result;

// Parse advertisement data from your BLE library
if (parser.parse(adv_data, adv_len, result) && result.valid) {
  switch (result.type) {
    case BEACON_TYPE_IBEACON:
      Serial.print("iBeacon UUID: ");
      Serial.println(result.getIBeacon().uuid);
      Serial.print("Major: ");
      Serial.println(result.getIBeacon().major);
      Serial.print("Minor: ");
      Serial.println(result.getIBeacon().minor);
      break;

    case BEACON_TYPE_EDDYSTONE_UID:
      // Handle Eddystone UID
      break;

    case BEACON_TYPE_EDDYSTONE_URL:
      Serial.print("URL: ");
      Serial.println(result.getEddystoneURL().url);
      break;

    case BEACON_TYPE_ALTBEACON:
      // Handle AltBeacon
      break;
  }
}
```

### Bluefruit Library Adapter

For Adafruit Bluefruit libraries, use the provided adapter:

```cpp
#include "bluefruit.h"
#include "BLEBeaconParser/adapters/BluefruitAdapter.h"

BluefruitBeaconParser parser;

void scan_callback(ble_gap_evt_adv_report_t* report) {
  BeaconData result;
  if (parser.parse(report, result) && result.valid) {
    // Handle parsed beacon
  }
}
```

## Development

### Running Tests

```bash
# Run tests
pio test -e native

# Run tests with verbose output
pio test -e native -v
```

### Code Formatting

```bash
# Format all code
./scripts/format.sh

# Check formatting (CI)
./scripts/check-format.sh
```

### Static Analysis

```bash
# Run clang-tidy
pio check -e native
```

### Docker Testing

```bash
# Build Docker image
docker build -t ble-beacon-parser-test .

# Run tests in Docker
docker run --rm ble-beacon-parser-test
```

## Supported Formats

### iBeacon (Apple)
- UUID (16 bytes)
- Major (2 bytes)
- Minor (2 bytes)
- TX Power (1 byte)

### Eddystone (Google)
- **UID**: Namespace ID (10 bytes) + Instance ID (6 bytes)
- **URL**: Encoded URL string
- **TLM**: Battery voltage, temperature, advertisement count, uptime

### AltBeacon (Radius Networks)
- Beacon ID (16 bytes)
- Major (2 bytes)
- Minor (2 bytes)
- Reference RSSI (1 byte)
- Manufacturer Reserved (1 byte)

## License

MIT License - see LICENSE file for details

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Ensure tests pass: `pio test -e native`
5. Format code: `./scripts/format.sh`
6. Submit a pull request

## CI/CD

The project uses GitHub Actions for continuous integration:
- **Tests**: Runs unit tests on every push/PR
- **Linting**: Checks code quality with clang-tidy
- **Format Check**: Verifies code formatting
- **Docker Test**: Runs tests in Docker environment

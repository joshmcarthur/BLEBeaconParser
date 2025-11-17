# Contributing Guide

## Development Setup

### Prerequisites

- PlatformIO Core (install via pip: `pip install platformio`)
- clang-format (for code formatting)
- clang-tidy (for static analysis - installed automatically by PlatformIO)

### Installing Tools

**macOS:**
```bash
brew install clang-format
```

**Ubuntu/Debian:**
```bash
sudo apt-get install clang-format clang-tidy
```

**Windows:**
Download from [LLVM releases](https://releases.llvm.org/)

## Development Workflow

### 1. Running Tests

```bash
# Run all tests
pio test -e native

# Run with verbose output
pio test -e native -v

# Run specific test file
pio test -e native --filter test_iBeacon
```

### 2. Code Formatting

```bash
# Format all code files
./scripts/format.sh

# Check if code is formatted (for CI)
./scripts/check-format.sh
```

### 3. Static Analysis

```bash
# Run clang-tidy checks
pio check -e native

# Run with specific severity
pio check -e native --fail-on-defect=high
```

### 4. Before Committing

1. Run tests: `pio test -e native`
2. Format code: `./scripts/format.sh`
3. Check formatting: `./scripts/check-format.sh`
4. Run linting: `pio check -e native`

## Code Style

The project uses:
- **clang-format**: Google style with Arduino-friendly modifications
- **clang-tidy**: Focused on bug detection and code quality

### Key Style Guidelines

- 2-space indentation
- 100 character line limit
- CamelCase for classes/structs
- camelBack for functions/variables
- UPPER_CASE for constants/macros

## Testing

### Writing Tests

1. Create test files in `test/` directory
2. Use Unity test framework (already configured)
3. Follow naming convention: `test_*.cpp`
4. Register tests in `test_main.cpp`

Example:
```cpp
#include <unity.h>
#include "BLEBeaconParser.h"

void test_my_feature() {
    // Test code
    TEST_ASSERT_TRUE(some_condition);
}
```

### Test Structure

- `test_main.cpp`: Test runner and registration
- `test_*.cpp`: Individual test suites
- `mock_arduino.h`: Mock Arduino String class for native testing

## Pull Request Process

1. Fork the repository
2. Create a feature branch: `git checkout -b feature/my-feature`
3. Make your changes
4. Add tests for new functionality
5. Ensure all tests pass: `pio test -e native`
6. Format code: `./scripts/format.sh`
7. Commit with descriptive messages
8. Push to your fork
9. Create a pull request

## CI/CD

GitHub Actions will automatically:
- Run tests on all pushes and PRs
- Check code formatting
- Run static analysis
- Test in Docker environment

All checks must pass before merging.

## Questions?

Open an issue or start a discussion in the repository.


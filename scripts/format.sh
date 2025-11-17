#!/bin/bash
# Format all C++ source files using clang-format

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}Formatting C++ files with clang-format...${NC}"

# Find all C++ files
FILES=$(find lib test -name "*.cpp" -o -name "*.h" | sort)

if [ -z "$FILES" ]; then
    echo -e "${YELLOW}No C++ files found to format.${NC}"
    exit 0
fi

FORMATTED=0
FAILED=0

for file in $FILES; do
    if clang-format -i "$file"; then
        echo -e "${GREEN}✓${NC} Formatted: $file"
        FORMATTED=$((FORMATTED + 1))
    else
        echo -e "${RED}✗${NC} Failed to format: $file"
        FAILED=$((FAILED + 1))
    fi
done

echo ""
echo -e "${GREEN}Formatting complete!${NC}"
echo -e "  Formatted: ${GREEN}$FORMATTED${NC} files"
if [ $FAILED -gt 0 ]; then
    echo -e "  Failed: ${RED}$FAILED${NC} files"
    exit 1
fi


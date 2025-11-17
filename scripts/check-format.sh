#!/bin/bash
# Check if code is properly formatted using clang-format

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}Checking code formatting with clang-format...${NC}"

# Find all C++ files
FILES=$(find lib test -name "*.cpp" -o -name "*.h" | sort)

if [ -z "$FILES" ]; then
    echo -e "${YELLOW}No C++ files found to check.${NC}"
    exit 0
fi

UNFORMATTED=0

for file in $FILES; do
    if ! clang-format "$file" | diff -q "$file" - > /dev/null; then
        echo -e "${RED}✗${NC} Not formatted: $file"
        UNFORMATTED=$((UNFORMATTED + 1))
    else
        echo -e "${GREEN}✓${NC} Formatted: $file"
    fi
done

echo ""
if [ $UNFORMATTED -gt 0 ]; then
    echo -e "${RED}Formatting check failed!${NC}"
    echo -e "  ${RED}$UNFORMATTED${NC} file(s) need formatting"
    echo -e "  Run ${YELLOW}./scripts/format.sh${NC} to fix"
    exit 1
else
    echo -e "${GREEN}All files are properly formatted!${NC}"
fi


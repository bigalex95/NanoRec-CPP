#!/bin/bash

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Check if executable exists
if [ ! -f "build/NanoRec-CPP" ]; then
    echo -e "${RED}❌ Executable not found!${NC}"
    echo -e "${YELLOW}Please run './scripts/build.sh' first to compile the project.${NC}"
    exit 1
fi

# Check if executable is actually executable
if [ ! -x "build/NanoRec-CPP" ]; then
    echo -e "${YELLOW}⚠️  Making executable...${NC}"
    chmod +x build/NanoRec-CPP
fi

# Run the application
echo -e "${GREEN}🚀 Running NanoRec-CPP...${NC}"
echo ""
./build/NanoRec-CPP "$@"
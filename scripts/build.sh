#!/bin/bash

# Exit on error
set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}🔨 Building NanoRec-CPP...${NC}"

# Check if build directory exists
if [ ! -d "build" ]; then
    echo -e "${YELLOW}📁 Creating build directory...${NC}"
    mkdir -p build
fi

# Navigate to build directory
cd build

# Run CMake
echo -e "${GREEN}⚙️  Configuring with CMake...${NC}"
if ! cmake ..; then
    echo -e "${RED}❌ CMake configuration failed!${NC}"
    exit 1
fi

# Build the project
echo -e "${GREEN}🔧 Compiling...${NC}"
if ! cmake --build . --config Release; then
    echo -e "${RED}❌ Build failed!${NC}"
    exit 1
fi

echo -e "${GREEN}✅ Build completed successfully!${NC}"
echo -e "${YELLOW}Run './scripts/run.sh' to execute the application.${NC}"
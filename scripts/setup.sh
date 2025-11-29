#!/bin/bash

# Exit on error
set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}🎬 Setting up NanoRec-CPP...${NC}"

# 1. Create Directory Structure
echo -e "${YELLOW}📁 Creating directories...${NC}"
mkdir -p src/core
mkdir -p include/core
mkdir -p vendor/miniaudio
mkdir -p vendor/imgui
mkdir -p .github/workflows

# 2. Download MiniAudio (Single Header)
if [ ! -f "vendor/miniaudio/miniaudio.h" ]; then
    echo -e "${YELLOW}⬇️  Downloading MiniAudio...${NC}"
    if command -v wget &> /dev/null; then
        wget -q https://raw.githubusercontent.com/mackron/miniaudio/master/miniaudio.h -O vendor/miniaudio/miniaudio.h
    elif command -v curl &> /dev/null; then
        curl -s https://raw.githubusercontent.com/mackron/miniaudio/master/miniaudio.h -o vendor/miniaudio/miniaudio.h
    else
        echo -e "${RED}❌ Neither wget nor curl found. Please install one of them.${NC}"
        exit 1
    fi
    echo -e "${GREEN}✅ MiniAudio downloaded${NC}"
else
    echo -e "${GREEN}✅ MiniAudio already exists${NC}"
fi

# 3. Clone ImGui (Docking Branch)
if [ -d "vendor/imgui/.git" ]; then
    echo -e "${GREEN}✅ ImGui already exists${NC}"
else
    echo -e "${YELLOW}⬇️  Cloning ImGui (Docking Branch)...${NC}"
    if ! git clone -b docking https://github.com/ocornut/imgui.git vendor/imgui; then
        echo -e "${RED}❌ Failed to clone ImGui${NC}"
        exit 1
    fi
    echo -e "${GREEN}✅ ImGui cloned successfully${NC}"
fi

# 4. Make scripts executable
echo -e "${YELLOW}🔐 Setting script permissions...${NC}"
chmod +x scripts/build.sh scripts/run.sh scripts/setup.sh 2>/dev/null || true

echo ""
echo -e "${GREEN}✅ Project setup complete!${NC}"
echo -e "${YELLOW}Next steps:${NC}"
echo -e "  1. Run ${GREEN}./scripts/build.sh${NC} to compile the project"
echo -e "  2. Run ${GREEN}./scripts/run.sh${NC} to execute the application"
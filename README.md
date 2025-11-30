# NanoRec-CPP 🎥

![Build Status](https://github.com/bigalex95/NanoRec-CPP/actions/workflows/build.yml/badge.svg)
![License](https://img.shields.io/badge/license-MIT-blue.svg)
![C++](https://img.shields.io/badge/std-c%2B%2B20-blue)

**NanoRec-CPP** is a high-performance, cross-platform screen recorder designed to run efficiently on legacy hardware (tested on 2014 laptops) while providing modern AI features.

## 🚀 Features (Planned)

- [ ] **Core:** Low-latency Screen Capture (GDI / X11).
- [ ] **Video:** Hardware-accelerated encoding via FFmpeg piping.
- [ ] **Audio:** System audio & Microphone capture using MiniAudio.
- [ ] **UI:** Lightweight Overlay using Dear ImGui & OpenGL.
- [ ] **AI:** Local audio transcription and summarization using Whisper.cpp.

## 🛠 Tech Stack

- **Language:** C++20
- **Build System:** CMake
- **GUI:** Dear ImGui + GLFW
- **Audio:** MiniAudio
- **Video Backend:** FFmpeg (CLI Pipe)
- **AI Backend:** Whisper.cpp (quantized models)

## 📥 Building from Source

### Prerequisites

- **Linux:** `sudo apt install build-essential cmake libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev libgl1-mesa-dev ffmpeg`
- **Windows:** Visual Studio 2022 (C++ Desktop Dev), CMake, FFmpeg installed in PATH.

### Build Steps

```bash
git clone https://github.com/bigalex95/NanoRec-CPP.git
cd NanoRec-CPP
./scripts/setup.sh      # Download dependencies
./scripts/build.sh      # Build the project
./scripts/run.sh        # Run the application
```

Or manually:

```bash
mkdir build && cd build
cmake ..
cmake --build . --config Release

# Executables will be in:
# - build/bin/NanoRec-CPP (main application)
# - build/bin/tests/test_capture (test executable)
```

## 🤝 Contributing

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

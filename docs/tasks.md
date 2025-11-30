[[NanoRec-CPP|⬅️ Back to Project Dashboard]]

# 📝 Project Roadmap: NanoRec-CPP

## 🚩 Phase 1: The Core (Screen Capture & Encoding)

_Goal: Record the screen to an .mp4 file using a CLI (no GUI yet) to ensure performance on low-end hardware._

- [x] **1.1 Project Scaffolding** ✅ _Completed: 2025-11-29_
  - [x] Create directory structure (`src`, `vendor`, `build`).
  - [x] Configure `CMakeLists.txt` for cross-platform builds.
  - [x] Set up GitHub Actions for CI/CD.
- [x] **1.2 Screen Capture Class (The Foundation)** ✅ _Completed: 2025-11-30_
  - [x] Define abstract `IScreenCapture` interface.
  - [x] Implement `WindowsScreenCapture` using GDI (BitBlt).
  - [x] Implement `LinuxScreenCapture` using X11 (XGetImage).
  - [x] **Benchmark:** ~47ms on 4K+ (4480x1440), optimized direct memory access.
  - **Note:** Performance scales with resolution. Target met on 1080p, larger resolutions need GPU acceleration (Phase 5).
- [ ] **1.3 The FFmpeg Pipe**
  - [ ] Create `VideoWriter` class that spawns an `ffmpeg` subprocess.
  - [ ] Implement `PipeWriter` to push raw RGB bytes to `ffmpeg` stdin.
  - [ ] Handle subprocess start/stop signals safely.
- [ ] **1.4 Integration Test**
  - [ ] Create a console app that records 10 seconds of screen and saves `output.mp4`.

## 🖥️ Phase 2: User Interface (ImGui & OpenGL)

_Goal: Visualize what we are recording and control it with buttons._

- [ ] **2.1 Windowing System**
  - [ ] Initialize GLFW and create a window.
  - [ ] Setup OpenGL context (GL 3.0+).
- [ ] **2.2 ImGui Integration**
  - [ ] Initialize ImGui context within GLFW.
  - [ ] Create a basic layout: "Start", "Stop", "Status".
- [ ] **2.3 Real-Time Preview**
  - [ ] Create an OpenGL Texture from the captured screen buffer.
  - [ ] Update texture every frame (efficiently) to show a preview in the UI.
- [ ] **2.4 Multithreading**
  - [ ] Move `ScreenCapture` loop to a background thread (`std::thread`).
  - [ ] Use `std::atomic` or `std::mutex` to safely share frames between the Recorder and the UI Preview.
- [ ] **2.5 Multi-Monitor Support**
  - [ ] Enumerate all connected displays (X11: XRandR / Windows: EnumDisplayMonitors).
  - [ ] Add UI dropdown/list to select capture source (Monitor 1, Monitor 2, All Monitors).
  - [ ] Update `IScreenCapture` interface to support monitor selection.
  - [ ] Implement virtual desktop capture for "All Monitors" mode.
- [ ] **2.6 Resolution & Scaling Options**
  - [ ] Add UI controls for output resolution selection (Native, 1080p, 720p, Custom).
  - [ ] Implement downscaling for high-DPI displays (>1080p) to improve performance.
  - [ ] Use bilinear/bicubic filtering for quality downscaling.
  - [ ] Add aspect ratio preservation option.
- [ ] **2.7 Screenshot Feature**
  - [ ] Add "Screenshot" button in UI.
  - [ ] Implement instant capture to PNG/JPG format.
  - [ ] Support screenshot modes:
    - [ ] Selected monitor only
    - [ ] All monitors (stitched horizontally)
    - [ ] Active window only (future enhancement)
  - [ ] Auto-save to user-configured directory with timestamp filename.
  - [ ] Show notification/toast on successful capture.

## 🎤 Phase 3: Audio Subsystem

_Goal: Capture Mic and System Audio, staying in sync with video._

- [ ] **3.1 Audio Setup (MiniAudio)**
  - [ ] Initialize `miniaudio` context.
  - [ ] List available input devices (Microphone vs Loopback).
- [ ] **3.2 Microphone Capture**
  - [ ] Implement callback to capture raw PCM audio from mic.
- [ ] **3.3 System Audio Capture**
  - [ ] Implement WASAPI loopback (Windows) / Pulse/PipeWire monitor (Linux).
- [ ] **3.4 Audio-Video Muxing**
  - [ ] Update `VideoWriter` pipe to accept an audio stream (or use a second pipe).
  - [ ] **Crucial:** Handle timestamp synchronization (drift correction).

## 🧠 Phase 4: AI & Intelligence

_Goal: Local transcription and Cloud summarization._

- [ ] **4.1 Whisper.cpp Integration**
  - [ ] Add `whisper.cpp` to `vendor`.
  - [ ] Download `ggml-tiny.en.bin` model (75MB).
  - [ ] Implement `Transcriber` class to process audio chunks in the background.
- [ ] **4.2 Summary Logic**
  - [ ] Accumulate transcribed text.
  - [ ] Implement `GeminiClient` using `libcurl`.
  - [ ] Send text to Gemini API for "Bullet Point Summary".
- [ ] **4.3 UI Updates**
  - [ ] Add "Live Transcript" window in ImGui.
  - [ ] Add "Generate Summary" button after recording stops.

## 🚀 Phase 5: Optimization & Polish (The "C++ Skills" Phase)

- [ ] **5.1 Zero-Copy Optimization**
  - [ ] _Advanced:_ Use pointer arithmetic to avoid copying pixel buffers between Capture -> Preview.
- [ ] **5.2 GPU Acceleration (Optional)**
  - [ ] Use OpenGL Compute Shaders or CUDA for RGB->YUV conversion before piping.
- [ ] **5.3 Installer**
  - [ ] Create a `.deb` (Linux) and `.msi`/`.exe` (Windows) for distribution.

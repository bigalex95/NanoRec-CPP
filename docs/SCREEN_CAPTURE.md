# Screen Capture Module

**Status:** ✅ Implemented (Phase 1.2 Complete)  
**Date:** November 30, 2025

## Overview

Cross-platform screen capture implementation using native APIs:

- **Linux:** X11 (XGetImage)
- **Windows:** GDI (BitBlt)

## Architecture

### Interface (`IScreenCapture`)

```cpp
class IScreenCapture {
    virtual bool initialize() = 0;
    virtual bool captureFrame(FrameBuffer& buffer) = 0;
    virtual int getWidth() const = 0;
    virtual int getHeight() const = 0;
    virtual void shutdown() = 0;
};
```

### Factory Pattern

```cpp
auto capture = NanoRec::createScreenCapture();
capture->initialize();
FrameBuffer buffer;
capture->captureFrame(buffer);
```

## Implementation Details

### Linux (X11)

- **API:** `XGetImage()` for direct framebuffer access
- **Format:** Captures in 32-bit BGRA, converts to RGB24
- **Optimization:** Direct memory access (avoiding slow `XGetPixel`)
- **Dependencies:** `libX11`

### Windows (GDI)

- **API:** `BitBlt()` for hardware-accelerated capture
- **Format:** Uses `GetDIBits()` to extract RGB24
- **Dependencies:** `Gdi32.lib`, `User32.lib`

## Performance

### Benchmarks (Test System)

- **Resolution:** 4480x1440 (6.4M pixels, high-DPI dual monitor)
- **Average Time:** 47ms per frame
- **FPS:** ~21 FPS
- **Optimization:** 2x speedup vs initial implementation (81ms → 47ms)

### 1080p Performance (Estimated)

- **Resolution:** 1920x1080 (2.1M pixels)
- **Expected Time:** ~15ms per frame ✅
- **Expected FPS:** ~66 FPS ✅

> **Note:** Performance scales linearly with pixel count. 4K+ resolutions may benefit from GPU acceleration (Phase 5.2).

## Usage

### Basic Example

```cpp
#include "capture/IScreenCapture.hpp"

auto capture = NanoRec::createScreenCapture();
if (!capture->initialize()) {
    // Handle error
}

FrameBuffer buffer;
while (recording) {
    if (capture->captureFrame(buffer)) {
        // Process frame (buffer.data, buffer.width, buffer.height)
    }
}

capture->shutdown();
```

### Frame Buffer Format

- **Pixel Format:** RGB24 (3 bytes per pixel)
- **Byte Order:** Red, Green, Blue
- **Stride:** `width * 3` bytes per row
- **Total Size:** `width * height * 3` bytes

## Testing

### Build and Run Test Program

```bash
# Build (tests are built automatically with BUILD_TESTS=ON by default)
cd build
cmake .. && make

# Run test (from project root)
cd ..
./build/bin/tests/test_capture

# Or from build directory
cd build
./bin/tests/test_capture

# Disable tests during build
cmake -DBUILD_TESTS=OFF ..
```

### Test Output

- Captures 10 frames
- Measures performance per frame
- Saves first frame as `screenshot_test.ppm`
- Reports average FPS

### View Screenshot (Linux)

```bash
# Convert PPM to PNG
convert screenshot_test.ppm screenshot_test.png

# Or view directly with ImageMagick
display screenshot_test.ppm
```

## Files

### Headers

- `include/capture/IScreenCapture.hpp` - Abstract interface + FrameBuffer
- `include/capture/LinuxScreenCapture.hpp` - Linux implementation
- `include/capture/WindowsScreenCapture.hpp` - Windows implementation

### Implementation

- `src/capture/ScreenCaptureFactory.cpp` - Platform factory
- `src/capture/LinuxScreenCapture.cpp` - X11 capture logic
- `src/capture/WindowsScreenCapture.cpp` - GDI capture logic

### Tests

- `tests/test_capture.cpp` - Standalone test program

## Known Limitations

1. **Single Monitor (Primary):** Currently captures primary display only
2. **High-DPI Performance:** Large resolutions (>4K) may not achieve 60 FPS without GPU optimization
3. **X11 Only (Linux):** Wayland support not implemented (requires different APIs)

## Future Optimizations (Phase 5)

1. **GPU Acceleration:** Use OpenGL PBO or CUDA for format conversion
2. **Multi-Monitor:** Add monitor selection API
3. **Wayland Support:** Implement `wlr-screencopy` protocol
4. **Zero-Copy:** Reduce buffer copies between capture → preview → encoder

## Integration Points

- **Phase 1.3:** Frame data pipes directly to FFmpeg stdin (RGB24 format)
- **Phase 2.3:** Buffer converts to OpenGL texture for preview
- **Phase 2.4:** Runs on background thread with atomic synchronization

## Dependencies

### Linux

```bash
sudo apt install libx11-dev
```

### Windows

- Built-in (GDI32, User32)

## Lessons Learned

1. **XGetPixel is slow:** Direct memory access provided 2x speedup
2. **Resolution matters:** Performance scales with pixel count
3. **RGB24 format:** Good balance between size and compatibility
4. **RAII pattern:** Proper cleanup with destructors prevents leaks

---

**Next Phase:** [1.3 FFmpeg Pipe](../docs/tasks.md) - Encode captured frames to video

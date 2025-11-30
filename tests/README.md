# Tests

This directory contains test executables for validating individual components of NanoRec-CPP.

## Building Tests

Tests are built automatically by default through CMake:

```bash
cd build
cmake ..
make
```

All test executables are placed in `build/bin/tests/`.

### Disable Tests

To skip building tests:

```bash
cmake -DBUILD_TESTS=OFF ..
make
```

## Available Tests

### `test_capture` - Screen Capture Validation

**Purpose:** Validates cross-platform screen capture functionality and measures performance.

**What it does:**

- Initializes platform-specific screen capture (X11 on Linux, GDI on Windows)
- Captures 10 consecutive frames
- Measures capture time per frame
- Saves the first frame as `screenshot_test.ppm`
- Reports average FPS and performance metrics

**Run:**

```bash
# From project root
./build/bin/tests/test_capture

# Or from build directory
cd build
./bin/tests/test_capture
```

**Expected Output:**

```
[INFO] === Screen Capture Test ===
[INFO] Creating Linux X11 screen capture instance
[INFO] Initializing Linux X11 screen capture...
[INFO] Screen capture initialized: 1920x1080
[INFO] Screen dimensions: 1920x1080
[INFO] Capturing 10 frames for performance test...
[INFO] Frame 1 captured in 14.523 ms
...
[INFO] === Performance Summary ===
[INFO] Average capture time: 15.234 ms
[INFO] Estimated FPS: 65.641234
✓ Performance target met (<16ms for 60 FPS)
```

**Performance Targets:**

- **1080p:** < 16ms per frame (60 FPS) ✅
- **1440p:** < 20ms per frame (50 FPS)
- **4K:** < 30ms per frame (30 FPS)

> **Note:** High-resolution displays (4K+) may exceed target times without GPU acceleration.

**Artifacts:**

- `screenshot_test.ppm` - First captured frame in PPM format (viewable with ImageMagick)

**View Screenshot:**

```bash
# Convert to PNG
convert screenshot_test.ppm screenshot_test.png

# Or view directly
display screenshot_test.ppm  # Linux with ImageMagick
```

## Test Structure

Tests are organized as standalone executables that:

1. Link only required components (minimal dependencies)
2. Report results via Logger system
3. Return exit code 0 on success, non-zero on failure
4. Generate artifacts in `build/` directory

## Adding New Tests

To add a new test executable:

1. **Create test file** in `tests/` directory (e.g., `tests/test_audio.cpp`)

2. **Update CMakeLists.txt:**

```cmake
if(BUILD_TESTS)
    # Existing tests...

    # New test
    add_executable(test_audio
        tests/test_audio.cpp
        src/core/Logger.cpp
        # Add other required sources
    )

    target_include_directories(test_audio PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

    target_link_libraries(test_audio PRIVATE
        # Add required libraries
    )

    set_target_properties(test_audio PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/tests
    )
endif()
```

3. **Build and run:**

```bash
cd build
make
./bin/tests/test_audio
```

## Continuous Integration

Tests can be run in CI/CD pipelines:

```bash
# Build
cmake -B build -S .
cmake --build build

# Run all tests
./build/bin/tests/test_capture
# Add more tests here
```

## Future Tests

Planned test executables:

- `test_audio` - Audio capture validation (Phase 3)
- `test_encoder` - FFmpeg pipe validation (Phase 1.3)
- `test_whisper` - Transcription validation (Phase 4)
- `test_performance` - End-to-end performance benchmarks

---

**Current Status:** 1 test available (test_capture)

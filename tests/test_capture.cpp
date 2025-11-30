/**
 * @file test_capture.cpp
 * @brief Simple test program to verify screen capture functionality
 * @author NanoRec-CPP Team
 * @date 2025-11-30
 *
 * Build and run via CMake:
 *   cd build && cmake .. && make
 *   ./bin/tests/test_capture
 */ \
#include "capture/IScreenCapture.hpp"
#include "core/Logger.hpp"
#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>

using namespace NanoRec;

/**
 * @brief Save frame buffer as PPM image (simple uncompressed format)
 * @param buffer Frame buffer to save
 * @param filename Output filename
 */
void saveFrameAsPPM(const FrameBuffer &buffer, const std::string &filename)
{
    std::ofstream file(filename, std::ios::binary);
    if (!file)
    {
        Logger::log(Logger::Level::ERROR_LEVEL, "Failed to open file: " + filename);
        return;
    }

    // PPM header (P6 = binary RGB)
    file << "P6\n"
         << buffer.width << " " << buffer.height << "\n255\n";

    // Write pixel data
    file.write(reinterpret_cast<const char *>(buffer.data), buffer.size);

    Logger::log(Logger::Level::INFO, "Saved frame to: " + filename);
}

int main()
{
    Logger::log(Logger::Level::INFO, "=== Screen Capture Test ===");

    // Create screen capture instance
    auto capture = createScreenCapture();
    if (!capture)
    {
        Logger::log(Logger::Level::ERROR_LEVEL, "Failed to create screen capture instance");
        return 1;
    }

    // Initialize
    if (!capture->initialize())
    {
        Logger::log(Logger::Level::ERROR_LEVEL, "Failed to initialize screen capture");
        return 1;
    }

    Logger::log(Logger::Level::INFO, "Screen dimensions: " + std::to_string(capture->getWidth()) + "x" +
                                         std::to_string(capture->getHeight()));

    // Allocate frame buffer
    FrameBuffer buffer;

    // Capture 10 frames and measure performance
    Logger::log(Logger::Level::INFO, "\nCapturing 10 frames for performance test...");

    double totalTime = 0.0;
    int frameCount = 10;

    for (int i = 0; i < frameCount; ++i)
    {
        std::chrono::steady_clock::time_point startTime = std::chrono::steady_clock::now();

        if (!capture->captureFrame(buffer))
        {
            Logger::log(Logger::Level::ERROR_LEVEL, "Failed to capture frame " + std::to_string(i + 1));
            continue;
        }

        std::chrono::steady_clock::time_point endTime = std::chrono::steady_clock::now();
        std::chrono::microseconds duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        double ms = duration.count() / 1000.0;
        totalTime += ms;

        Logger::log(Logger::Level::INFO, "Frame " + std::to_string(i + 1) + " captured in " +
                                             std::to_string(ms) + " ms");

        // Save first frame as sample
        if (i == 0)
        {
            saveFrameAsPPM(buffer, "screenshot_test.ppm");
        }

        // Small delay between captures
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // Performance summary
    double avgTime = totalTime / frameCount;
    double fps = 1000.0 / avgTime;

    Logger::log(Logger::Level::INFO, "\n=== Performance Summary ===");
    Logger::log(Logger::Level::INFO, "Average capture time: " + std::to_string(avgTime) + " ms");
    Logger::log(Logger::Level::INFO, "Estimated FPS: " + std::to_string(fps));

    if (avgTime < 16.0)
    {
        Logger::log(Logger::Level::INFO, "✓ Performance target met (<16ms for 60 FPS)");
    }
    else
    {
        Logger::log(Logger::Level::WARNING, "✗ Performance target NOT met (target: <16ms)");
    }

    // Cleanup
    capture->shutdown();

    Logger::log(Logger::Level::INFO, "\nTest completed successfully!");
    Logger::log(Logger::Level::INFO, "Check 'screenshot_test.ppm' to verify capture quality");

    return 0;
}

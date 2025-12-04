/**
 * @file test_recording.cpp
 * @brief Integration test for screen recording with FFmpeg encoding
 * @author NanoRec-CPP Team
 * @date 2025-12-04
 *
 * This test captures screen frames for 10 seconds and encodes them to output.mp4
 * using FFmpeg. It demonstrates the complete recording pipeline:
 *   Screen Capture → FFmpeg Encoder → MP4 File
 *
 * Build and run via CMake:
 *   cd build && cmake .. && make
 *   ./bin/tests/test_recording
 */

#include "capture/IScreenCapture.hpp"
#include "core/FFmpegVideoWriter.hpp"
#include "core/Logger.hpp"
#include <iostream>
#include <chrono>
#include <thread>

using namespace NanoRec;

int main()
{
    Logger::log(Logger::Level::INFO, "=== Screen Recording Integration Test ===");
    Logger::log(Logger::Level::INFO, "This test will record your screen for 10 seconds");

    // Configuration
    const int RECORDING_DURATION_SEC = 10;
    const int TARGET_FPS = 30;
    const int FRAME_DELAY_MS = 1000 / TARGET_FPS;

    // Step 1: Initialize screen capture
    Logger::log(Logger::Level::INFO, "\n[1/4] Initializing screen capture...");
    auto capture = createScreenCapture();
    if (!capture)
    {
        Logger::log(Logger::Level::ERROR_LEVEL, "Failed to create screen capture instance");
        return 1;
    }

    if (!capture->initialize())
    {
        Logger::log(Logger::Level::ERROR_LEVEL, "Failed to initialize screen capture");
        return 1;
    }

    int width = capture->getWidth();
    int height = capture->getHeight();
    Logger::log(Logger::Level::INFO, "Screen resolution: " + std::to_string(width) + "x" + std::to_string(height));

    // Step 2: Initialize video writer
    Logger::log(Logger::Level::INFO, "\n[2/4] Initializing FFmpeg video encoder...");
    FFmpegVideoWriter videoWriter;
    
    VideoConfig config;
    config.width = width;
    config.height = height;
    config.fps = TARGET_FPS;
    config.output = "output.mp4";

    if (!videoWriter.initialize(config))
    {
        Logger::log(Logger::Level::ERROR_LEVEL, "Failed to initialize video writer");
        capture->shutdown();
        return 1;
    }

    Logger::log(Logger::Level::INFO, "Encoder configured: " + std::to_string(TARGET_FPS) + " FPS, H.264 codec");

    // Step 3: Record frames
    Logger::log(Logger::Level::INFO, "\n[3/4] Recording started...");
    Logger::log(Logger::Level::INFO, "Duration: " + std::to_string(RECORDING_DURATION_SEC) + " seconds");
    Logger::log(Logger::Level::INFO, "Output: output.mp4");
    Logger::log(Logger::Level::INFO, "");

    FrameBuffer buffer;
    int totalFrames = RECORDING_DURATION_SEC * TARGET_FPS;
    int frameCount = 0;
    
    double totalCaptureTime = 0.0;
    double totalWriteTime = 0.0;

    auto recordingStart = std::chrono::steady_clock::now();

    for (int i = 0; i < totalFrames; ++i)
    {
        auto frameStart = std::chrono::steady_clock::now();

        // Capture frame
        auto captureStart = std::chrono::steady_clock::now();
        if (!capture->captureFrame(buffer))
        {
            Logger::log(Logger::Level::ERROR_LEVEL, "Failed to capture frame " + std::to_string(i + 1));
            continue;
        }
        auto captureEnd = std::chrono::steady_clock::now();
        double captureMs = std::chrono::duration_cast<std::chrono::microseconds>(captureEnd - captureStart).count() / 1000.0;
        totalCaptureTime += captureMs;

        // Write frame to video
        auto writeStart = std::chrono::steady_clock::now();
        if (!videoWriter.writeFrame(buffer.data, buffer.size))
        {
            Logger::log(Logger::Level::ERROR_LEVEL, "Failed to write frame " + std::to_string(i + 1));
            continue;
        }
        auto writeEnd = std::chrono::steady_clock::now();
        double writeMs = std::chrono::duration_cast<std::chrono::microseconds>(writeEnd - writeStart).count() / 1000.0;
        totalWriteTime += writeMs;

        frameCount++;

        // Progress indicator (every 30 frames = 1 second at 30 FPS)
        if ((i + 1) % TARGET_FPS == 0)
        {
            int secondsElapsed = (i + 1) / TARGET_FPS;
            Logger::log(Logger::Level::INFO, 
                "Progress: " + std::to_string(secondsElapsed) + "/" + std::to_string(RECORDING_DURATION_SEC) + "s " +
                "(Capture: " + std::to_string(captureMs) + "ms, Write: " + std::to_string(writeMs) + "ms)");
        }

        // Maintain target frame rate
        auto frameEnd = std::chrono::steady_clock::now();
        auto frameDuration = std::chrono::duration_cast<std::chrono::milliseconds>(frameEnd - frameStart);
        int sleepTime = FRAME_DELAY_MS - static_cast<int>(frameDuration.count());
        
        if (sleepTime > 0)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
        }
    }

    auto recordingEnd = std::chrono::steady_clock::now();
    double totalRecordingTime = std::chrono::duration_cast<std::chrono::milliseconds>(recordingEnd - recordingStart).count() / 1000.0;

    // Step 4: Finalize and cleanup
    Logger::log(Logger::Level::INFO, "\n[4/4] Finalizing video encoding...");
    
    if (!videoWriter.finalize())
    {
        Logger::log(Logger::Level::ERROR_LEVEL, "Failed to finalize video");
    }

    capture->shutdown();

    // Performance summary
    Logger::log(Logger::Level::INFO, "\n=== Recording Complete ===");
    Logger::log(Logger::Level::INFO, "Total frames captured: " + std::to_string(frameCount));
    Logger::log(Logger::Level::INFO, "Total recording time: " + std::to_string(totalRecordingTime) + " seconds");
    
    if (frameCount > 0)
    {
        double avgCaptureTime = totalCaptureTime / frameCount;
        double avgWriteTime = totalWriteTime / frameCount;
        double avgTotalTime = avgCaptureTime + avgWriteTime;
        double actualFPS = frameCount / totalRecordingTime;

        Logger::log(Logger::Level::INFO, "\n=== Performance Metrics ===");
        Logger::log(Logger::Level::INFO, "Average capture time: " + std::to_string(avgCaptureTime) + " ms/frame");
        Logger::log(Logger::Level::INFO, "Average write time: " + std::to_string(avgWriteTime) + " ms/frame");
        Logger::log(Logger::Level::INFO, "Average total time: " + std::to_string(avgTotalTime) + " ms/frame");
        Logger::log(Logger::Level::INFO, "Actual FPS: " + std::to_string(actualFPS));
        Logger::log(Logger::Level::INFO, "Target FPS: " + std::to_string(TARGET_FPS));

        if (actualFPS >= TARGET_FPS * 0.9) // Within 10% of target
        {
            Logger::log(Logger::Level::INFO, "✓ Performance target met!");
        }
        else
        {
            Logger::log(Logger::Level::WARNING, "✗ Performance below target (consider reducing resolution or FPS)");
        }
    }

    Logger::log(Logger::Level::INFO, "\n=== Next Steps ===");
    Logger::log(Logger::Level::INFO, "1. Check that output.mp4 was created");
    Logger::log(Logger::Level::INFO, "2. Play the video with: ffplay output.mp4");
    Logger::log(Logger::Level::INFO, "   or any media player (VLC, mpv, etc.)");
    Logger::log(Logger::Level::INFO, "3. Verify the video shows your screen content");

    return 0;
}

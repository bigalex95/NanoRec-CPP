#include "core/CaptureThread.hpp"
#include "core/Logger.hpp"
#include "core/FrameScaler.hpp"
#include <chrono>
#include <iomanip>
#include <sstream>

namespace NanoRec
{

    CaptureThread::CaptureThread()
    {
    }

    CaptureThread::~CaptureThread()
    {
        stop();
    }

    bool CaptureThread::start(IScreenCapture *screenCapture, ThreadSafeFrameBuffer *frameBuffer)
    {
        if (m_running.load())
        {
            Logger::error("Capture thread already running");
            return false;
        }

        if (!screenCapture || !frameBuffer)
        {
            Logger::error("Invalid screen capture or frame buffer");
            return false;
        }

        m_screenCapture = screenCapture;
        m_frameBuffer = frameBuffer;
        m_shouldStop.store(false);
        m_running.store(true);

        // Start capture thread
        m_thread = std::thread(&CaptureThread::captureLoop, this);

        Logger::info("Capture thread started");
        return true;
    }

    void CaptureThread::stop()
    {
        if (!m_running.load())
        {
            return;
        }

        Logger::info("Stopping capture thread...");
        m_shouldStop.store(true);

        // Stop recording if active
        if (m_recording.load())
        {
            stopRecording();
        }

        // Wait for thread to finish
        if (m_thread.joinable())
        {
            m_thread.join();
        }

        m_running.store(false);
        Logger::info("Capture thread stopped");
    }

    bool CaptureThread::startRecording(const std::string &filename, int fps, 
                                       int targetWidth, int targetHeight)
    {
        if (m_recording.load())
        {
            Logger::error("Already recording");
            return false;
        }

        if (!m_screenCapture)
        {
            Logger::error("No screen capture available");
            return false;
        }

        m_recordingFilename = filename;
        m_recordingFPS = fps;

        // Get capture dimensions
        int captureWidth = m_screenCapture->getWidth();
        int captureHeight = m_screenCapture->getHeight();

        // Determine recording dimensions
        if (targetWidth == 0 || targetHeight == 0)
        {
            // Use native resolution
            m_recordingWidth = captureWidth;
            m_recordingHeight = captureHeight;
            m_useScaling = false;
        }
        else
        {
            // Use target resolution with scaling
            m_recordingWidth = targetWidth;
            m_recordingHeight = targetHeight;
            m_useScaling = (targetWidth != captureWidth || targetHeight != captureHeight);
        }

        // Create video writer
        m_videoWriter = std::make_unique<FFmpegVideoWriter>();

        VideoConfig config(m_recordingWidth, m_recordingHeight, fps, filename);
        if (!m_videoWriter->initialize(config))
        {
            Logger::error("Failed to initialize video writer");
            m_videoWriter.reset();
            return false;
        }

        m_recording.store(true);
        
        std::string scalingInfo = m_useScaling ? 
            " (scaled from " + std::to_string(captureWidth) + "x" + std::to_string(captureHeight) + ")" : "";
        
        Logger::info("Recording started: " + filename + " (" + 
                    std::to_string(m_recordingWidth) + "x" + std::to_string(m_recordingHeight) + 
                    " @ " + std::to_string(fps) + " FPS)" + scalingInfo);
        return true;
    }

    void CaptureThread::stopRecording()
    {
        if (!m_recording.load())
        {
            return;
        }

        m_recording.store(false);

        if (m_videoWriter)
        {
            m_videoWriter->finalize();
            m_videoWriter.reset();
            Logger::info("Recording stopped: " + m_recordingFilename);
        }
    }

    void CaptureThread::captureLoop()
    {
        Logger::info("Capture loop started");

        FrameBuffer captureBuffer;
        captureBuffer.allocate(m_screenCapture->getWidth(), m_screenCapture->getHeight());
        
        FrameBuffer scaledBuffer;  // For scaled recording

        auto lastFrameTime = std::chrono::high_resolution_clock::now();
        int frameCount = 0;
        auto fpsUpdateTime = lastFrameTime;

        while (!m_shouldStop.load())
        {
            auto frameStart = std::chrono::high_resolution_clock::now();

            // Capture frame
            if (m_screenCapture->captureFrame(captureBuffer))
            {
                // Push to frame buffer for preview
                m_frameBuffer->pushFrame(captureBuffer);

                // Write to video if recording
                if (m_recording.load() && m_videoWriter)
                {
                    if (m_useScaling)
                    {
                        // Scale frame before encoding
                        if (FrameScaler::scaleFrame(captureBuffer, scaledBuffer, 
                                                   m_recordingWidth, m_recordingHeight))
                        {
                            m_videoWriter->writeFrame(scaledBuffer.data, scaledBuffer.size);
                        }
                    }
                    else
                    {
                        // Write directly without scaling
                        m_videoWriter->writeFrame(captureBuffer.data, captureBuffer.size);
                    }
                }

                frameCount++;
            }

            // Calculate FPS every second
            auto now = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - fpsUpdateTime).count();
            if (elapsed >= 1000)
            {
                double fps = frameCount * 1000.0 / elapsed;
                m_currentFPS.store(fps);
                frameCount = 0;
                fpsUpdateTime = now;
            }

            // Target frame time for desired FPS
            int targetFrameTimeMs = 1000 / m_recordingFPS;
            auto frameEnd = std::chrono::high_resolution_clock::now();
            auto frameDuration = std::chrono::duration_cast<std::chrono::milliseconds>(frameEnd - frameStart).count();

            // Sleep if we're ahead of schedule
            if (frameDuration < targetFrameTimeMs)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(targetFrameTimeMs - frameDuration));
            }
        }

        Logger::info("Capture loop ended");
    }

} // namespace NanoRec

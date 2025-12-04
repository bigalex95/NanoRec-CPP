#pragma once

#include "capture/IScreenCapture.hpp"
#include "core/ThreadSafeFrameBuffer.hpp"
#include "core/FFmpegVideoWriter.hpp"
#include <atomic>
#include <thread>
#include <memory>

namespace NanoRec
{

    /**
     * @brief Background thread for screen capture and recording
     *
     * Runs screen capture in a separate thread to keep UI responsive.
     * Optionally records frames to video file via FFmpegVideoWriter.
     */
    class CaptureThread
    {
    public:
        CaptureThread();
        ~CaptureThread();

        /**
         * @brief Start the capture thread
         * @param screenCapture Screen capture instance
         * @param frameBuffer Thread-safe frame buffer for preview
         * @return true if started successfully
         */
        bool start(IScreenCapture *screenCapture, ThreadSafeFrameBuffer *frameBuffer);

        /**
         * @brief Stop the capture thread
         */
        void stop();

        /**
         * @brief Start recording to file
         * @param filename Output filename
         * @param fps Frames per second
         * @param targetWidth Target width (0 = native resolution)
         * @param targetHeight Target height (0 = native resolution)
         * @return true if recording started
         */
        bool startRecording(const std::string &filename, int fps = 30, 
                          int targetWidth = 0, int targetHeight = 0);

        /**
         * @brief Stop recording
         */
        void stopRecording();

        /**
         * @brief Check if thread is running
         */
        bool isRunning() const { return m_running.load(); }

        /**
         * @brief Check if currently recording
         */
        bool isRecording() const { return m_recording.load(); }

        /**
         * @brief Get current FPS
         */
        double getCurrentFPS() const { return m_currentFPS.load(); }

    private:
        void captureLoop();

        std::thread m_thread;
        std::atomic<bool> m_running{false};
        std::atomic<bool> m_shouldStop{false};
        std::atomic<bool> m_recording{false};
        std::atomic<double> m_currentFPS{0.0};

        IScreenCapture *m_screenCapture{nullptr};
        ThreadSafeFrameBuffer *m_frameBuffer{nullptr};
        std::unique_ptr<FFmpegVideoWriter> m_videoWriter;

        std::string m_recordingFilename;
        int m_recordingFPS{30};
        
        // Scaling support
        int m_recordingWidth{0};
        int m_recordingHeight{0};
        bool m_useScaling{false};
    };

} // namespace NanoRec

#pragma once

#include "capture/IScreenCapture.hpp"
#include <atomic>
#include <mutex>
#include <memory>

namespace NanoRec
{

    /**
     * @brief Thread-safe double-buffered frame storage
     *
     * Allows one thread to write frames while another reads them
     * without blocking. Uses double buffering with atomic swap.
     */
    class ThreadSafeFrameBuffer
    {
    public:
        ThreadSafeFrameBuffer();
        ~ThreadSafeFrameBuffer();

        /**
         * @brief Initialize buffers with given dimensions
         * @param width Frame width
         * @param height Frame height
         */
        void initialize(int width, int height);

        /**
         * @brief Push a new frame (called by capture thread)
         * @param frame Frame data to copy
         * @return true if successful
         */
        bool pushFrame(const FrameBuffer &frame);

        /**
         * @brief Get the latest frame for reading (called by UI thread)
         * @param outFrame Output frame buffer
         * @return true if new frame available
         */
        bool getLatestFrame(FrameBuffer &outFrame);

        /**
         * @brief Check if a new frame is available
         */
        bool hasNewFrame() const { return m_hasNewFrame.load(); }

        /**
         * @brief Get frame dimensions
         */
        int getWidth() const { return m_width; }
        int getHeight() const { return m_height; }

    private:
        // Double buffer
        FrameBuffer m_buffers[2];
        std::atomic<int> m_writeIndex{0};
        std::atomic<int> m_readIndex{1};
        std::atomic<bool> m_hasNewFrame{false};

        std::mutex m_swapMutex;

        int m_width{0};
        int m_height{0};
    };

} // namespace NanoRec

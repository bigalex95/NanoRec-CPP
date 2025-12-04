#include "core/ThreadSafeFrameBuffer.hpp"
#include "core/Logger.hpp"
#include <cstring>

namespace NanoRec
{

    ThreadSafeFrameBuffer::ThreadSafeFrameBuffer()
    {
    }

    ThreadSafeFrameBuffer::~ThreadSafeFrameBuffer()
    {
    }

    void ThreadSafeFrameBuffer::initialize(int width, int height)
    {
        m_width = width;
        m_height = height;

        // Allocate both buffers
        m_buffers[0].allocate(width, height);
        m_buffers[1].allocate(width, height);

        Logger::info("ThreadSafeFrameBuffer initialized: " + std::to_string(width) + "x" + std::to_string(height));
    }

    bool ThreadSafeFrameBuffer::pushFrame(const FrameBuffer &frame)
    {
        if (frame.data == nullptr || frame.width != m_width || frame.height != m_height)
        {
            return false;
        }

        // Get current write buffer
        int writeIdx = m_writeIndex.load();
        FrameBuffer &writeBuffer = m_buffers[writeIdx];

        // Copy frame data
        std::memcpy(writeBuffer.data, frame.data, frame.size);

        // Swap buffers atomically
        {
            std::lock_guard<std::mutex> lock(m_swapMutex);
            int oldWrite = m_writeIndex.load();
            int oldRead = m_readIndex.load();

            m_writeIndex.store(oldRead);
            m_readIndex.store(oldWrite);
        }

        m_hasNewFrame.store(true);
        return true;
    }

    bool ThreadSafeFrameBuffer::getLatestFrame(FrameBuffer &outFrame)
    {
        if (!m_hasNewFrame.load())
        {
            return false;
        }

        // Get current read buffer
        int readIdx = m_readIndex.load();
        const FrameBuffer &readBuffer = m_buffers[readIdx];

        // Ensure output buffer is allocated
        if (outFrame.data == nullptr)
        {
            outFrame.allocate(m_width, m_height);
        }

        // Copy frame data
        std::memcpy(outFrame.data, readBuffer.data, readBuffer.size);
        outFrame.width = readBuffer.width;
        outFrame.height = readBuffer.height;
        outFrame.stride = readBuffer.stride;
        outFrame.size = readBuffer.size;

        m_hasNewFrame.store(false);
        return true;
    }

} // namespace NanoRec

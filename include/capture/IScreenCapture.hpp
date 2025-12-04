/**
 * @file IScreenCapture.hpp
 * @brief Abstract interface for cross-platform screen capture
 * @author NanoRec-CPP Team
 * @date 2025-11-30
 */

#ifndef NANOREC_ISCREENCAPTURE_HPP
#define NANOREC_ISCREENCAPTURE_HPP

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>
#include "MonitorInfo.hpp"

namespace NanoRec
{

    /**
     * @struct FrameBuffer
     * @brief Container for captured frame data
     */
    struct FrameBuffer
    {
        uint8_t *data; ///< Raw pixel data (RGB24 format: 3 bytes per pixel)
        size_t size;   ///< Total buffer size in bytes
        int width;     ///< Frame width in pixels
        int height;    ///< Frame height in pixels
        int stride;    ///< Number of bytes per row (may include padding)

        FrameBuffer() : data(nullptr), size(0), width(0), height(0), stride(0) {}

        /**
         * @brief Allocate buffer for frame data
         * @param w Width in pixels
         * @param h Height in pixels
         */
        void allocate(int w, int h)
        {
            width = w;
            height = h;
            stride = width * 3; // RGB24 format
            size = stride * height;
            data = new uint8_t[size];
        }

        /**
         * @brief Free allocated buffer
         */
        void free()
        {
            if (data)
            {
                delete[] data;
                data = nullptr;
            }
            size = 0;
        }

        ~FrameBuffer()
        {
            free();
        }

        // Prevent copying (use move semantics if needed)
        FrameBuffer(const FrameBuffer &) = delete;
        FrameBuffer &operator=(const FrameBuffer &) = delete;

        // Allow moving
        FrameBuffer(FrameBuffer &&other) noexcept
            : data(other.data), size(other.size), width(other.width),
              height(other.height), stride(other.stride)
        {
            other.data = nullptr;
            other.size = 0;
        }

        FrameBuffer &operator=(FrameBuffer &&other) noexcept
        {
            if (this != &other)
            {
                free();
                data = other.data;
                size = other.size;
                width = other.width;
                height = other.height;
                stride = other.stride;
                other.data = nullptr;
                other.size = 0;
            }
            return *this;
        }
    };

    /**
     * @class IScreenCapture
     * @brief Abstract interface for screen capture implementations
     *
     * Provides platform-independent API for capturing screen frames.
     * Platform-specific implementations: WindowsScreenCapture (GDI), LinuxScreenCapture (X11)
     */
    class IScreenCapture
    {
    public:
        /**
         * @brief Initialize the screen capture system
         * @return true if initialization succeeded, false otherwise
         */
        virtual bool initialize() = 0;

        /**
         * @brief Capture a single frame from the screen
         * @param buffer FrameBuffer to store captured data
         * @return true if capture succeeded, false otherwise
         *
         * @note This method must complete in <16ms for 60 FPS operation
         */
        virtual bool captureFrame(FrameBuffer &buffer) = 0;

        /**
         * @brief Get the width of the capture area
         * @return Width in pixels
         */
        virtual int getWidth() const = 0;

        /**
         * @brief Get the height of the capture area
         * @return Height in pixels
         */
        virtual int getHeight() const = 0;

        /**
         * @brief Enumerate all available monitors
         * @return Vector of MonitorInfo structures
         */
        virtual std::vector<MonitorInfo> enumerateMonitors() = 0;

        /**
         * @brief Select which monitor to capture
         * @param monitorId Monitor ID (-1 for all monitors, 0+ for specific monitor)
         * @return true if monitor selection succeeded
         */
        virtual bool selectMonitor(int monitorId) = 0;

        /**
         * @brief Get currently selected monitor ID
         * @return Monitor ID (-1 for all monitors, 0+ for specific monitor)
         */
        virtual int getCurrentMonitor() const = 0;

        /**
         * @brief Shutdown and cleanup resources
         */
        virtual void shutdown() = 0;

        /**
         * @brief Virtual destructor for proper cleanup
         */
        virtual ~IScreenCapture() = default;
    };

    /**
     * @brief Factory function to create platform-specific screen capture instance
     * @return Unique pointer to IScreenCapture implementation
     */
    std::unique_ptr<IScreenCapture> createScreenCapture();

} // namespace NanoRec

#endif // NANOREC_ISCREENCAPTURE_HPP

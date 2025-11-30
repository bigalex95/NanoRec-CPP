/**
 * @file LinuxScreenCapture.cpp
 * @brief Linux X11-based screen capture implementation
 * @author NanoRec-CPP Team
 * @date 2025-11-30
 */

#ifdef __linux__

#include "capture/LinuxScreenCapture.hpp"
#include "core/Logger.hpp"
#include <chrono>
#include <cstring>

namespace NanoRec
{

    LinuxScreenCapture::LinuxScreenCapture()
        : m_display(nullptr), m_rootWindow(0), m_screen(0), m_width(0), m_height(0), m_initialized(false)
    {
    }

    LinuxScreenCapture::~LinuxScreenCapture()
    {
        shutdown();
    }

    bool LinuxScreenCapture::initialize()
    {
        Logger::log(Logger::Level::INFO, "Initializing Linux X11 screen capture...");

        // Open connection to X server
        m_display = XOpenDisplay(nullptr);
        if (!m_display)
        {
            Logger::log(Logger::Level::ERROR, "Failed to open X display");
            return false;
        }

        // Get default screen
        m_screen = DefaultScreen(m_display);
        m_rootWindow = RootWindow(m_display, m_screen);

        // Get screen dimensions
        XWindowAttributes attrs;
        if (XGetWindowAttributes(m_display, m_rootWindow, &attrs) == 0)
        {
            Logger::log(Logger::Level::ERROR, "Failed to get root window attributes");
            XCloseDisplay(m_display);
            m_display = nullptr;
            return false;
        }

        m_width = attrs.width;
        m_height = attrs.height;

        Logger::log(Logger::Level::INFO, "Screen capture initialized: " + std::to_string(m_width) + "x" +
                                             std::to_string(m_height));

        m_initialized = true;
        return true;
    }

    bool LinuxScreenCapture::captureFrame(FrameBuffer &buffer)
    {
        if (!m_initialized)
        {
            Logger::log(Logger::Level::ERROR, "Screen capture not initialized");
            return false;
        }

        auto startTime = std::chrono::steady_clock::now();

        // Capture screen using XGetImage
        XImage *ximage = XGetImage(
            m_display,
            m_rootWindow,
            0, 0,              // x, y offset
            m_width, m_height, // width, height
            AllPlanes,         // plane mask
            ZPixmap            // format
        );

        if (!ximage)
        {
            Logger::log(Logger::Level::ERROR, "XGetImage failed");
            return false;
        }

        // Allocate buffer if needed
        if (buffer.width != m_width || buffer.height != m_height || !buffer.data)
        {
            buffer.free();
            buffer.allocate(m_width, m_height);
        }

        // Convert X11 image to RGB24
        convertToRGB24(ximage, buffer);

        // Cleanup X11 image
        XDestroyImage(ximage);

        // Performance measurement
        auto endTime = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

        if (duration.count() > 16)
        {
            Logger::log(Logger::Level::WARNING, "Warning: Frame capture took " + std::to_string(duration.count()) +
                                                    "ms (target: <16ms for 60 FPS)");
        }

        return true;
    }

    void LinuxScreenCapture::convertToRGB24(XImage *ximage, FrameBuffer &buffer)
    {
        // X11 image format varies by system, typically 32-bit BGRA or 24-bit BGR
        // Optimized: Direct memory access instead of XGetPixel (much faster)

        const int bytesPerPixel = ximage->bits_per_pixel / 8;
        uint8_t *src = reinterpret_cast<uint8_t *>(ximage->data);
        uint8_t *dest = buffer.data;

        // Fast conversion using direct memory access
        if (ximage->bits_per_pixel == 32)
        {
            // 32-bit format (BGRA): Convert to RGB24
            for (int y = 0; y < m_height; ++y)
            {
                uint8_t *row = src + (y * ximage->bytes_per_line);
                for (int x = 0; x < m_width; ++x)
                {
                    uint8_t b = row[0];
                    uint8_t g = row[1];
                    uint8_t r = row[2];
                    // Skip alpha (row[3])

                    *dest++ = r;
                    *dest++ = g;
                    *dest++ = b;

                    row += 4; // Move to next pixel
                }
            }
        }
        else if (ximage->bits_per_pixel == 24)
        {
            // 24-bit format (BGR): Convert to RGB24
            for (int y = 0; y < m_height; ++y)
            {
                uint8_t *row = src + (y * ximage->bytes_per_line);
                for (int x = 0; x < m_width; ++x)
                {
                    uint8_t b = row[0];
                    uint8_t g = row[1];
                    uint8_t r = row[2];

                    *dest++ = r;
                    *dest++ = g;
                    *dest++ = b;

                    row += 3; // Move to next pixel
                }
            }
        }
        else
        {
            // Fallback: Use XGetPixel (slower but compatible)
            Logger::log(Logger::Level::WARNING, "Using slow XGetPixel fallback for " +
                                                    std::to_string(ximage->bits_per_pixel) + "-bit format");

            for (int y = 0; y < m_height; ++y)
            {
                for (int x = 0; x < m_width; ++x)
                {
                    unsigned long pixel = XGetPixel(ximage, x, y);
                    *dest++ = (pixel >> 16) & 0xFF; // R
                    *dest++ = (pixel >> 8) & 0xFF;  // G
                    *dest++ = (pixel >> 0) & 0xFF;  // B
                }
            }
        }
    }

    void LinuxScreenCapture::shutdown()
    {
        if (m_display)
        {
            Logger::log(Logger::Level::INFO, "Shutting down X11 screen capture");
            XCloseDisplay(m_display);
            m_display = nullptr;
        }
        m_initialized = false;
    }

} // namespace NanoRec

#endif // __linux__

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
        : m_display(nullptr), m_rootWindow(0), m_screen(0), m_width(0), m_height(0), m_initialized(false),
          m_selectedMonitor(-1), m_captureX(0), m_captureY(0), m_captureWidth(0), m_captureHeight(0)
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
            Logger::log(Logger::Level::ERROR_LEVEL, "Failed to open X display");
            return false;
        }

        // Get default screen
        m_screen = DefaultScreen(m_display);
        m_rootWindow = RootWindow(m_display, m_screen);

        // Get screen dimensions
        XWindowAttributes attrs;
        if (XGetWindowAttributes(m_display, m_rootWindow, &attrs) == 0)
        {
            Logger::log(Logger::Level::ERROR_LEVEL, "Failed to get root window attributes");
            XCloseDisplay(m_display);
            m_display = nullptr;
            return false;
        }

        m_width = attrs.width;
        m_height = attrs.height;

        // Enumerate monitors
        enumerateMonitorsXRandR();

        // Default to all monitors (-1)
        updateCaptureRegion();

        Logger::log(Logger::Level::INFO, "Screen capture initialized: " + std::to_string(m_width) + "x" +
                                             std::to_string(m_height));

        m_initialized = true;
        return true;
    }

    bool LinuxScreenCapture::captureFrame(FrameBuffer &buffer)
    {
        if (!m_initialized)
        {
            Logger::log(Logger::Level::ERROR_LEVEL, "Screen capture not initialized");
            return false;
        }

        auto startTime = std::chrono::steady_clock::now();

        // Capture screen using XGetImage (use capture region)
        XImage *ximage = XGetImage(
            m_display,
            m_rootWindow,
            m_captureX, m_captureY,              // x, y offset
            m_captureWidth, m_captureHeight,     // width, height
            AllPlanes,                            // plane mask
            ZPixmap                               // format
        );

        if (!ximage)
        {
            Logger::log(Logger::Level::ERROR_LEVEL, "XGetImage failed");
            return false;
        }

        // Allocate buffer if needed
        if (buffer.width != m_captureWidth || buffer.height != m_captureHeight || !buffer.data)
        {
            buffer.free();
            buffer.allocate(m_captureWidth, m_captureHeight);
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
            for (int y = 0; y < m_captureHeight; ++y)
            {
                uint8_t *row = src + (y * ximage->bytes_per_line);
                for (int x = 0; x < m_captureWidth; ++x)
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
            for (int y = 0; y < m_captureHeight; ++y)
            {
                uint8_t *row = src + (y * ximage->bytes_per_line);
                for (int x = 0; x < m_captureWidth; ++x)
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

            for (int y = 0; y < m_captureHeight; ++y)
            {
                for (int x = 0; x < m_captureWidth; ++x)
                {
                    unsigned long pixel = XGetPixel(ximage, x, y);
                    *dest++ = (pixel >> 16) & 0xFF; // R
                    *dest++ = (pixel >> 8) & 0xFF;  // G
                    *dest++ = (pixel >> 0) & 0xFF;  // B
                }
            }
        }
    }

    void LinuxScreenCapture::enumerateMonitorsXRandR()
    {
        m_monitors.clear();

        // Check if XRandR extension is available
        int eventBase, errorBase;
        if (!XRRQueryExtension(m_display, &eventBase, &errorBase))
        {
            Logger::log(Logger::Level::WARNING, "XRandR extension not available, multi-monitor support disabled");
            return;
        }

        // Get screen resources
        XRRScreenResources *resources = XRRGetScreenResources(m_display, m_rootWindow);
        if (!resources)
        {
            Logger::log(Logger::Level::WARNING, "Failed to get XRandR screen resources");
            return;
        }

        // Enumerate outputs (monitors)
        int monitorId = 0;
        for (int i = 0; i < resources->noutput; ++i)
        {
            XRROutputInfo *outputInfo = XRRGetOutputInfo(m_display, resources, resources->outputs[i]);
            if (!outputInfo)
                continue;

            // Only include connected outputs
            if (outputInfo->connection == RR_Connected && outputInfo->crtc)
            {
                XRRCrtcInfo *crtcInfo = XRRGetCrtcInfo(m_display, resources, outputInfo->crtc);
                if (crtcInfo)
                {
                    MonitorInfo monitor;
                    monitor.id = monitorId++;
                    monitor.name = std::string(outputInfo->name) + " (Monitor " + std::to_string(monitor.id + 1) + ")";
                    monitor.x = crtcInfo->x;
                    monitor.y = crtcInfo->y;
                    monitor.width = crtcInfo->width;
                    monitor.height = crtcInfo->height;
                    monitor.isPrimary = (i == 0); // First monitor is usually primary

                    m_monitors.push_back(monitor);

                    Logger::log(Logger::Level::INFO, "Found monitor: " + monitor.name + " at " +
                                                          std::to_string(monitor.x) + "," + std::to_string(monitor.y) +
                                                          " (" + std::to_string(monitor.width) + "x" +
                                                          std::to_string(monitor.height) + ")");

                    XRRFreeCrtcInfo(crtcInfo);
                }
            }

            XRRFreeOutputInfo(outputInfo);
        }

        XRRFreeScreenResources(resources);

        Logger::log(Logger::Level::INFO, "Enumerated " + std::to_string(m_monitors.size()) + " monitors");
    }

    std::vector<MonitorInfo> LinuxScreenCapture::enumerateMonitors()
    {
        return m_monitors;
    }

    bool LinuxScreenCapture::selectMonitor(int monitorId)
    {
        if (monitorId != -1 && (monitorId < 0 || monitorId >= static_cast<int>(m_monitors.size())))
        {
            Logger::log(Logger::Level::ERROR_LEVEL, "Invalid monitor ID: " + std::to_string(monitorId));
            return false;
        }

        m_selectedMonitor = monitorId;
        updateCaptureRegion();

        if (monitorId == -1)
        {
            Logger::log(Logger::Level::INFO, "Selected all monitors (virtual desktop)");
        }
        else
        {
            Logger::log(Logger::Level::INFO, "Selected monitor: " + m_monitors[monitorId].name);
        }

        return true;
    }

    void LinuxScreenCapture::updateCaptureRegion()
    {
        if (m_selectedMonitor == -1)
        {
            // Capture all monitors (entire virtual desktop)
            m_captureX = 0;
            m_captureY = 0;
            m_captureWidth = m_width;
            m_captureHeight = m_height;
        }
        else if (m_selectedMonitor >= 0 && m_selectedMonitor < static_cast<int>(m_monitors.size()))
        {
            // Capture specific monitor
            const MonitorInfo &monitor = m_monitors[m_selectedMonitor];
            m_captureX = monitor.x;
            m_captureY = monitor.y;
            m_captureWidth = monitor.width;
            m_captureHeight = monitor.height;
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

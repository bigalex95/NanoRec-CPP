/**
 * @file WindowsScreenCapture.cpp
 * @brief Windows GDI-based screen capture implementation
 * @author NanoRec-CPP Team
 * @date 2025-11-30
 */

#ifdef _WIN32

#include "capture/WindowsScreenCapture.hpp"
#include "core/Logger.hpp"
#include <chrono>
#include <string>

namespace NanoRec
{

    WindowsScreenCapture::WindowsScreenCapture()
        : m_screenDC(nullptr), m_memoryDC(nullptr), m_bitmap(nullptr), m_oldBitmap(nullptr), m_width(0), m_height(0), m_initialized(false)
    {
        ZeroMemory(&m_bitmapInfo, sizeof(BITMAPINFO));
    }

    WindowsScreenCapture::~WindowsScreenCapture()
    {
        shutdown();
    }

    bool WindowsScreenCapture::initialize()
    {
        Logger::log(Logger::Level::INFO, "Initializing Windows GDI screen capture...");

        // Get screen DC
        m_screenDC = GetDC(nullptr);
        if (!m_screenDC)
        {
            Logger::log(Logger::Level::ERROR_LEVEL, "Failed to get screen DC");
            return false;
        }

        // Get screen dimensions
        m_width = GetSystemMetrics(SM_CXSCREEN);
        m_height = GetSystemMetrics(SM_CYSCREEN);

        // Create compatible DC for memory operations
        m_memoryDC = CreateCompatibleDC(m_screenDC);
        if (!m_memoryDC)
        {
            Logger::log(Logger::Level::ERROR_LEVEL, "Failed to create compatible DC");
            ReleaseDC(nullptr, m_screenDC);
            m_screenDC = nullptr;
            return false;
        }

        // Create compatible bitmap
        m_bitmap = CreateCompatibleBitmap(m_screenDC, m_width, m_height);
        if (!m_bitmap)
        {
            Logger::log(Logger::Level::ERROR_LEVEL, "Failed to create compatible bitmap");
            DeleteDC(m_memoryDC);
            ReleaseDC(nullptr, m_screenDC);
            m_memoryDC = nullptr;
            m_screenDC = nullptr;
            return false;
        }

        // Select bitmap into memory DC
        m_oldBitmap = (HBITMAP)SelectObject(m_memoryDC, m_bitmap);

        // Setup bitmap info for DIB operations
        setupBitmapInfo();

        Logger::log(Logger::Level::INFO, "Screen capture initialized: " + std::to_string(m_width) + "x" +
                                             std::to_string(m_height));

        m_initialized = true;
        return true;
    }

    void WindowsScreenCapture::setupBitmapInfo()
    {
        m_bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        m_bitmapInfo.bmiHeader.biWidth = m_width;
        m_bitmapInfo.bmiHeader.biHeight = -m_height; // Negative for top-down DIB
        m_bitmapInfo.bmiHeader.biPlanes = 1;
        m_bitmapInfo.bmiHeader.biBitCount = 24; // RGB24
        m_bitmapInfo.bmiHeader.biCompression = BI_RGB;
        m_bitmapInfo.bmiHeader.biSizeImage = 0;
    }

    bool WindowsScreenCapture::captureFrame(FrameBuffer &buffer)
    {
        if (!m_initialized)
        {
            Logger::log(Logger::Level::ERROR_LEVEL, "Screen capture not initialized");
            return false;
        }

        auto startTime = std::chrono::steady_clock::now();

        // Capture screen to memory DC using BitBlt
        if (!BitBlt(m_memoryDC, 0, 0, m_width, m_height,
                    m_screenDC, 0, 0, SRCCOPY))
        {
            Logger::log(Logger::Level::ERROR_LEVEL, "BitBlt failed");
            return false;
        }

        // Allocate buffer if needed
        if (buffer.width != m_width || buffer.height != m_height || !buffer.data)
        {
            buffer.free();
            buffer.allocate(m_width, m_height);
        }

        // Get bitmap bits as DIB (Device Independent Bitmap)
        // This converts from device-specific format to RGB24
        int scanlines = GetDIBits(
            m_memoryDC,
            m_bitmap,
            0,             // First scan line
            m_height,      // Number of scan lines
            buffer.data,   // Buffer to receive bits
            &m_bitmapInfo, // Bitmap info
            DIB_RGB_COLORS // Color table format
        );

        if (scanlines == 0)
        {
            Logger::log(Logger::Level::ERROR_LEVEL, "GetDIBits failed");
            return false;
        }

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

    void WindowsScreenCapture::shutdown()
    {
        if (m_memoryDC)
        {
            if (m_oldBitmap)
            {
                SelectObject(m_memoryDC, m_oldBitmap);
                m_oldBitmap = nullptr;
            }
            DeleteDC(m_memoryDC);
            m_memoryDC = nullptr;
        }

        if (m_bitmap)
        {
            DeleteObject(m_bitmap);
            m_bitmap = nullptr;
        }

        if (m_screenDC)
        {
            ReleaseDC(nullptr, m_screenDC);
            m_screenDC = nullptr;
        }

        Logger::log(Logger::Level::INFO, "Shutting down Windows GDI screen capture");
        m_initialized = false;
    }

} // namespace NanoRec

#endif // _WIN32

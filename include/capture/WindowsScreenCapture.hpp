/**
 * @file WindowsScreenCapture.hpp
 * @brief Windows GDI-based screen capture implementation
 * @author NanoRec-CPP Team
 * @date 2025-11-30
 */

#ifndef NANOREC_WINDOWSSCREENCAPTURE_HPP
#define NANOREC_WINDOWSSCREENCAPTURE_HPP

#include "IScreenCapture.hpp"

#ifdef _WIN32

#include <windows.h>

namespace NanoRec
{

    /**
     * @class WindowsScreenCapture
     * @brief GDI-based screen capture for Windows systems
     *
     * Uses BitBlt to capture the desktop contents efficiently.
     * Designed for minimal latency to achieve 60 FPS capture.
     */
    class WindowsScreenCapture : public IScreenCapture
    {
    public:
        WindowsScreenCapture();
        ~WindowsScreenCapture() override;

        bool initialize() override;
        bool captureFrame(FrameBuffer &buffer) override;
        int getWidth() const override { return m_width; }
        int getHeight() const override { return m_height; }
        void shutdown() override;

    private:
        HDC m_screenDC;          ///< Device context for screen
        HDC m_memoryDC;          ///< Memory device context
        HBITMAP m_bitmap;        ///< Bitmap handle
        HBITMAP m_oldBitmap;     ///< Previous bitmap for cleanup
        int m_width;             ///< Screen width
        int m_height;            ///< Screen height
        bool m_initialized;      ///< Initialization state
        BITMAPINFO m_bitmapInfo; ///< Bitmap information structure

        /**
         * @brief Setup bitmap info structure for DIB operations
         */
        void setupBitmapInfo();
    };

} // namespace NanoRec

#endif // _WIN32

#endif // NANOREC_WINDOWSSCREENCAPTURE_HPP

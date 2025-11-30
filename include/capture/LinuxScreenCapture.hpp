/**
 * @file LinuxScreenCapture.hpp
 * @brief Linux X11-based screen capture implementation
 * @author NanoRec-CPP Team
 * @date 2025-11-30
 */

#ifndef NANOREC_LINUXSCREENCAPTURE_HPP
#define NANOREC_LINUXSCREENCAPTURE_HPP

#include "IScreenCapture.hpp"

#ifdef __linux__

#include <X11/Xlib.h>
#include <X11/Xutil.h>

namespace NanoRec
{

    /**
     * @class LinuxScreenCapture
     * @brief X11-based screen capture for Linux systems
     *
     * Uses XGetImage to capture the root window (desktop) contents.
     * Designed for minimal latency to achieve 60 FPS capture.
     */
    class LinuxScreenCapture : public IScreenCapture
    {
    public:
        LinuxScreenCapture();
        ~LinuxScreenCapture() override;

        bool initialize() override;
        bool captureFrame(FrameBuffer &buffer) override;
        int getWidth() const override { return m_width; }
        int getHeight() const override { return m_height; }
        void shutdown() override;

    private:
        Display *m_display;  ///< X11 display connection
        Window m_rootWindow; ///< Root window (desktop)
        int m_screen;        ///< Default screen number
        int m_width;         ///< Screen width
        int m_height;        ///< Screen height
        bool m_initialized;  ///< Initialization state

        /**
         * @brief Convert X11 image data to RGB24 format
         * @param ximage X11 image structure
         * @param buffer Destination buffer
         */
        void convertToRGB24(XImage *ximage, FrameBuffer &buffer);
    };

} // namespace NanoRec

#endif // __linux__

#endif // NANOREC_LINUXSCREENCAPTURE_HPP

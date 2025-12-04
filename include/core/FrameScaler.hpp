#pragma once

#include "capture/IScreenCapture.hpp"

namespace NanoRec
{

    /**
     * @class FrameScaler
     * @brief Utility for scaling video frames with quality filtering
     */
    class FrameScaler
    {
    public:
        /**
         * @brief Scale a frame using bilinear interpolation
         * @param source Source frame buffer
         * @param destination Destination frame buffer (must be pre-allocated)
         * @param targetWidth Target width in pixels
         * @param targetHeight Target height in pixels
         * @return true if scaling succeeded, false otherwise
         */
        static bool scaleFrame(
            const FrameBuffer &source,
            FrameBuffer &destination,
            int targetWidth,
            int targetHeight);

        /**
         * @brief Calculate scaled dimensions preserving aspect ratio
         * @param sourceWidth Source width in pixels
         * @param sourceHeight Source height in pixels
         * @param maxWidth Maximum width constraint
         * @param maxHeight Maximum height constraint
         * @param outWidth Output width (preserving aspect ratio)
         * @param outHeight Output height (preserving aspect ratio)
         */
        static void calculateScaledDimensions(
            int sourceWidth, int sourceHeight,
            int maxWidth, int maxHeight,
            int &outWidth, int &outHeight);

    private:
        /**
         * @brief Bilinear interpolation for a single pixel
         * @param source Source frame data
         * @param srcWidth Source width
         * @param srcHeight Source height
         * @param x Floating-point x coordinate
         * @param y Floating-point y coordinate
         * @param channel Color channel (0=R, 1=G, 2=B)
         * @return Interpolated pixel value
         */
        static uint8_t bilinearSample(
            const uint8_t *source,
            int srcWidth, int srcHeight,
            float x, float y,
            int channel);
    };

} // namespace NanoRec

#include "core/FrameScaler.hpp"
#include "core/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <cstring>

namespace NanoRec
{

    bool FrameScaler::scaleFrame(
        const FrameBuffer &source,
        FrameBuffer &destination,
        int targetWidth,
        int targetHeight)
    {
        if (!source.data || source.width <= 0 || source.height <= 0)
        {
            Logger::error("Invalid source frame for scaling");
            return false;
        }

        if (targetWidth <= 0 || targetHeight <= 0)
        {
            Logger::error("Invalid target dimensions for scaling");
            return false;
        }

        // Ensure destination buffer is allocated
        if (!destination.data || destination.width != targetWidth || destination.height != targetHeight)
        {
            destination.free();
            destination.allocate(targetWidth, targetHeight);
        }

        // Calculate scaling ratios
        float xRatio = static_cast<float>(source.width) / targetWidth;
        float yRatio = static_cast<float>(source.height) / targetHeight;

        // Perform bilinear scaling
        for (int y = 0; y < targetHeight; ++y)
        {
            for (int x = 0; x < targetWidth; ++x)
            {
                // Map destination pixel to source coordinates
                float srcX = x * xRatio;
                float srcY = y * yRatio;

                // Get destination pixel pointer
                int dstIdx = (y * targetWidth + x) * 3;

                // Sample each color channel with bilinear interpolation
                destination.data[dstIdx + 0] = bilinearSample(source.data, source.width, source.height, srcX, srcY, 0); // R
                destination.data[dstIdx + 1] = bilinearSample(source.data, source.width, source.height, srcX, srcY, 1); // G
                destination.data[dstIdx + 2] = bilinearSample(source.data, source.width, source.height, srcX, srcY, 2); // B
            }
        }

        return true;
    }

    void FrameScaler::calculateScaledDimensions(
        int sourceWidth, int sourceHeight,
        int maxWidth, int maxHeight,
        int &outWidth, int &outHeight)
    {
        // Calculate aspect ratio
        float aspectRatio = static_cast<float>(sourceWidth) / sourceHeight;

        // Start with max dimensions
        outWidth = maxWidth;
        outHeight = maxHeight;

        // Adjust to preserve aspect ratio
        if (outWidth > maxWidth)
        {
            outWidth = maxWidth;
            outHeight = static_cast<int>(outWidth / aspectRatio);
        }

        if (outHeight > maxHeight)
        {
            outHeight = maxHeight;
            outWidth = static_cast<int>(outHeight * aspectRatio);
        }

        // Ensure even dimensions (required for some video codecs)
        outWidth = (outWidth / 2) * 2;
        outHeight = (outHeight / 2) * 2;
    }

    uint8_t FrameScaler::bilinearSample(
        const uint8_t *source,
        int srcWidth, int srcHeight,
        float x, float y,
        int channel)
    {
        // Get integer coordinates
        int x0 = static_cast<int>(std::floor(x));
        int y0 = static_cast<int>(std::floor(y));
        int x1 = std::min(x0 + 1, srcWidth - 1);
        int y1 = std::min(y0 + 1, srcHeight - 1);

        // Clamp to valid range
        x0 = std::max(0, std::min(x0, srcWidth - 1));
        y0 = std::max(0, std::min(y0, srcHeight - 1));

        // Get fractional parts
        float fx = x - x0;
        float fy = y - y0;

        // Get pixel values at four corners
        int idx00 = (y0 * srcWidth + x0) * 3 + channel;
        int idx10 = (y0 * srcWidth + x1) * 3 + channel;
        int idx01 = (y1 * srcWidth + x0) * 3 + channel;
        int idx11 = (y1 * srcWidth + x1) * 3 + channel;

        uint8_t p00 = source[idx00];
        uint8_t p10 = source[idx10];
        uint8_t p01 = source[idx01];
        uint8_t p11 = source[idx11];

        // Bilinear interpolation
        float top = p00 * (1.0f - fx) + p10 * fx;
        float bottom = p01 * (1.0f - fx) + p11 * fx;
        float result = top * (1.0f - fy) + bottom * fy;

        return static_cast<uint8_t>(std::round(result));
    }

} // namespace NanoRec

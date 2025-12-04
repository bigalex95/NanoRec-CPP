#pragma once

#include "capture/IScreenCapture.hpp"
#include <string>

namespace NanoRec
{

    /**
     * @class ImageWriter
     * @brief Utility for saving frame buffers as image files
     */
    class ImageWriter
    {
    public:
        /**
         * @brief Save a frame buffer as PNG image
         * @param filename Output filename (e.g., "screenshot.png")
         * @param frame Frame buffer to save
         * @return true if saved successfully, false otherwise
         */
        static bool savePNG(const std::string &filename, const FrameBuffer &frame);

        /**
         * @brief Generate a timestamped filename
         * @param prefix Filename prefix (default: "screenshot")
         * @param extension File extension (default: ".png")
         * @return Timestamped filename (e.g., "screenshot_20231204_153045.png")
         */
        static std::string generateTimestampedFilename(
            const std::string &prefix = "screenshot",
            const std::string &extension = ".png");
    };

} // namespace NanoRec

#include "core/ImageWriter.hpp"
#include "core/Logger.hpp"
#include <chrono>
#include <iomanip>
#include <sstream>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

namespace NanoRec
{

    bool ImageWriter::savePNG(const std::string &filename, const FrameBuffer &frame)
    {
        if (!frame.data || frame.width <= 0 || frame.height <= 0)
        {
            Logger::error("Invalid frame buffer for PNG export");
            return false;
        }

        // stbi_write_png expects RGB data with stride = width * 3
        int stride = frame.width * 3;
        
        // Write PNG file
        int result = stbi_write_png(
            filename.c_str(),
            frame.width,
            frame.height,
            3,              // RGB (3 channels)
            frame.data,
            stride
        );

        if (result == 0)
        {
            Logger::error("Failed to write PNG file: " + filename);
            return false;
        }

        Logger::info("Screenshot saved: " + filename + " (" + 
                    std::to_string(frame.width) + "x" + std::to_string(frame.height) + ")");
        return true;
    }

    std::string ImageWriter::generateTimestampedFilename(
        const std::string &prefix,
        const std::string &extension)
    {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        
        std::stringstream ss;
        ss << prefix << "_" 
           << std::put_time(std::localtime(&time_t), "%Y%m%d_%H%M%S") 
           << extension;
        
        return ss.str();
    }

} // namespace NanoRec

/**
 * @file IVideoWriter.hpp
 * @brief Abstract interface for video encoding
 * @author NanoRec-CPP Team
 * @date 2025-12-04
 */

#ifndef NANOREC_IVIDEOWRITER_HPP
#define NANOREC_IVIDEOWRITER_HPP

#include <cstddef>
#include <cstdint>
#include <string>

namespace NanoRec
{

    /**
     * @struct VideoConfig
     * @brief Configuration parameters for video encoding
     */
    struct VideoConfig
    {
        int width;           ///< Video width in pixels
        int height;          ///< Video height in pixels
        int fps;             ///< Frames per second
        std::string output;  ///< Output file path

        VideoConfig() : width(1920), height(1080), fps(30), output("output.mp4") {}
        
        VideoConfig(int w, int h, int f, const std::string& out)
            : width(w), height(h), fps(f), output(out) {}
    };

    /**
     * @class IVideoWriter
     * @brief Abstract interface for video encoding implementations
     *
     * Provides platform-independent API for encoding video frames to file.
     * Implementations may use FFmpeg, hardware encoders, or other backends.
     */
    class IVideoWriter
    {
    public:
        /**
         * @brief Initialize the video writer with configuration
         * @param config Video encoding configuration
         * @return true if initialization succeeded, false otherwise
         */
        virtual bool initialize(const VideoConfig& config) = 0;

        /**
         * @brief Write a single frame to the video
         * @param frameData Raw RGB24 pixel data (width * height * 3 bytes)
         * @param dataSize Size of frame data in bytes
         * @return true if frame was written successfully, false otherwise
         */
        virtual bool writeFrame(const uint8_t* frameData, size_t dataSize) = 0;

        /**
         * @brief Finalize the video and close the file
         * @return true if finalization succeeded, false otherwise
         */
        virtual bool finalize() = 0;

        /**
         * @brief Check if the writer is currently active
         * @return true if writer is initialized and ready to write frames
         */
        virtual bool isActive() const = 0;

        /**
         * @brief Virtual destructor for proper cleanup
         */
        virtual ~IVideoWriter() = default;
    };

} // namespace NanoRec

#endif // NANOREC_IVIDEOWRITER_HPP

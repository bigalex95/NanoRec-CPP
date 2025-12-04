/**
 * @file FFmpegVideoWriter.hpp
 * @brief FFmpeg-based video encoder using subprocess pipe
 * @author NanoRec-CPP Team
 * @date 2025-12-04
 */

#ifndef NANOREC_FFMPEGVIDEOWRITER_HPP
#define NANOREC_FFMPEGVIDEOWRITER_HPP

#include "IVideoWriter.hpp"
#include <memory>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace NanoRec
{

    /**
     * @class FFmpegVideoWriter
     * @brief Video encoder using FFmpeg subprocess
     *
     * Spawns FFmpeg as a child process and pipes raw RGB frames to stdin.
     * FFmpeg handles encoding to H.264/MP4 format.
     *
     * @note Requires FFmpeg to be installed and available in system PATH
     */
    class FFmpegVideoWriter : public IVideoWriter
    {
    public:
        FFmpegVideoWriter();
        ~FFmpegVideoWriter() override;

        bool initialize(const VideoConfig& config) override;
        bool writeFrame(const uint8_t* frameData, size_t dataSize) override;
        bool finalize() override;
        bool isActive() const override;

    private:
        /**
         * @brief Check if FFmpeg is available in system PATH
         * @return true if FFmpeg is found, false otherwise
         */
        bool checkFFmpegAvailable();

        /**
         * @brief Spawn FFmpeg subprocess with appropriate arguments
         * @return true if process started successfully, false otherwise
         */
        bool spawnFFmpegProcess();

        /**
         * @brief Terminate FFmpeg subprocess gracefully
         */
        void terminateFFmpegProcess();

        /**
         * @brief Write data to FFmpeg stdin pipe
         * @param data Data to write
         * @param size Size of data in bytes
         * @return true if write succeeded, false otherwise
         */
        bool writeToPipe(const void* data, size_t size);

        VideoConfig m_config;
        bool m_active;

#ifdef _WIN32
        HANDLE m_stdinPipe;
        PROCESS_INFORMATION m_processInfo;
#else
        int m_pipeFd;
        pid_t m_processId;
#endif
    };

} // namespace NanoRec

#endif // NANOREC_FFMPEGVIDEOWRITER_HPP

/**
 * @file FFmpegVideoWriter.cpp
 * @brief Implementation of FFmpeg-based video encoder
 * @author NanoRec-CPP Team
 * @date 2025-12-04
 */

#include "core/FFmpegVideoWriter.hpp"
#include "core/Logger.hpp"
#include <sstream>
#include <cstring>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#endif

namespace NanoRec
{

    FFmpegVideoWriter::FFmpegVideoWriter()
        : m_active(false)
#ifdef _WIN32
        , m_stdinPipe(INVALID_HANDLE_VALUE)
#else
        , m_pipeFd(-1)
        , m_processId(-1)
#endif
    {
    }

    FFmpegVideoWriter::~FFmpegVideoWriter()
    {
        if (m_active)
        {
            finalize();
        }
    }

    bool FFmpegVideoWriter::checkFFmpegAvailable()
    {
#ifdef _WIN32
        // Try to run ffmpeg -version
        STARTUPINFOA si = {sizeof(si)};
        PROCESS_INFORMATION pi;
        si.dwFlags = STARTF_USESHOWWINDOW;
        si.wShowWindow = SW_HIDE;

        char cmd[] = "ffmpeg -version";
        BOOL success = CreateProcessA(
            nullptr, cmd, nullptr, nullptr, FALSE,
            CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi
        );

        if (success)
        {
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
            return true;
        }
        return false;
#else
        // Try to execute ffmpeg -version
        int result = system("ffmpeg -version > /dev/null 2>&1");
        return (result == 0);
#endif
    }

    bool FFmpegVideoWriter::initialize(const VideoConfig& config)
    {
        if (m_active)
        {
            Logger::log(Logger::Level::ERROR_LEVEL, "VideoWriter already initialized");
            return false;
        }

        // Validate configuration
        if (config.width <= 0 || config.height <= 0 || config.fps <= 0)
        {
            Logger::log(Logger::Level::ERROR_LEVEL, "Invalid video configuration");
            return false;
        }

        if (config.output.empty())
        {
            Logger::log(Logger::Level::ERROR_LEVEL, "Output path cannot be empty");
            return false;
        }

        // Check FFmpeg availability
        if (!checkFFmpegAvailable())
        {
            Logger::log(Logger::Level::ERROR_LEVEL, 
                "FFmpeg not found in PATH. Please install FFmpeg to use video recording.");
            return false;
        }

        m_config = config;

        // Spawn FFmpeg process
        if (!spawnFFmpegProcess())
        {
            Logger::log(Logger::Level::ERROR_LEVEL, "Failed to spawn FFmpeg process");
            return false;
        }

        m_active = true;
        Logger::log(Logger::Level::INFO, "FFmpeg video writer initialized: " + 
            std::to_string(config.width) + "x" + std::to_string(config.height) + 
            " @ " + std::to_string(config.fps) + " FPS");

        return true;
    }

    bool FFmpegVideoWriter::spawnFFmpegProcess()
    {
#ifdef _WIN32
        // Windows implementation using CreateProcess
        SECURITY_ATTRIBUTES saAttr;
        saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
        saAttr.bInheritHandle = TRUE;
        saAttr.lpSecurityDescriptor = nullptr;

        HANDLE stdinRead, stdinWrite;
        if (!CreatePipe(&stdinRead, &stdinWrite, &saAttr, 0))
        {
            Logger::log(Logger::Level::ERROR_LEVEL, "Failed to create pipe");
            return false;
        }

        // Ensure write handle is not inherited
        SetHandleInformation(stdinWrite, HANDLE_FLAG_INHERIT, 0);

        // Build FFmpeg command
        std::ostringstream cmd;
        cmd << "ffmpeg -y -f rawvideo -pixel_format rgb24 "
            << "-video_size " << m_config.width << "x" << m_config.height << " "
            << "-framerate " << m_config.fps << " "
            << "-i pipe:0 "
            << "-c:v libx264 -preset medium -crf 23 -pix_fmt yuv420p "
            << "\"" << m_config.output << "\"";

        std::string cmdStr = cmd.str();
        char* cmdLine = new char[cmdStr.length() + 1];
        strcpy(cmdLine, cmdStr.c_str());

        STARTUPINFOA si = {sizeof(si)};
        si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
        si.hStdInput = stdinRead;
        si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
        si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
        si.wShowWindow = SW_HIDE;

        ZeroMemory(&m_processInfo, sizeof(m_processInfo));

        BOOL success = CreateProcessA(
            nullptr, cmdLine, nullptr, nullptr, TRUE,
            CREATE_NO_WINDOW, nullptr, nullptr, &si, &m_processInfo
        );

        delete[] cmdLine;
        CloseHandle(stdinRead);

        if (!success)
        {
            CloseHandle(stdinWrite);
            Logger::log(Logger::Level::ERROR_LEVEL, "Failed to create FFmpeg process");
            return false;
        }

        m_stdinPipe = stdinWrite;
        return true;

#else
        // Linux implementation using fork/exec
        int pipeFds[2];
        if (pipe(pipeFds) == -1)
        {
            Logger::log(Logger::Level::ERROR_LEVEL, "Failed to create pipe: " + 
                std::string(strerror(errno)));
            return false;
        }

        pid_t pid = fork();
        if (pid == -1)
        {
            Logger::log(Logger::Level::ERROR_LEVEL, "Failed to fork process: " + 
                std::string(strerror(errno)));
            close(pipeFds[0]);
            close(pipeFds[1]);
            return false;
        }

        if (pid == 0)
        {
            // Child process
            close(pipeFds[1]); // Close write end
            dup2(pipeFds[0], STDIN_FILENO); // Redirect stdin to pipe
            close(pipeFds[0]);

            // Build FFmpeg arguments
            std::string videoSize = std::to_string(m_config.width) + "x" + std::to_string(m_config.height);
            std::string framerate = std::to_string(m_config.fps);

            execlp("ffmpeg", "ffmpeg",
                "-y",
                "-f", "rawvideo",
                "-pixel_format", "rgb24",
                "-video_size", videoSize.c_str(),
                "-framerate", framerate.c_str(),
                "-i", "pipe:0",
                "-c:v", "libx264",
                "-preset", "medium",
                "-crf", "23",
                "-pix_fmt", "yuv420p",
                m_config.output.c_str(),
                nullptr
            );

            // If exec fails
            Logger::log(Logger::Level::ERROR_LEVEL, "Failed to exec FFmpeg: " + 
                std::string(strerror(errno)));
            _exit(1);
        }

        // Parent process
        close(pipeFds[0]); // Close read end
        m_pipeFd = pipeFds[1];
        m_processId = pid;

        return true;
#endif
    }

    bool FFmpegVideoWriter::writeFrame(const uint8_t* frameData, size_t dataSize)
    {
        if (!m_active)
        {
            Logger::log(Logger::Level::ERROR_LEVEL, "VideoWriter not initialized");
            return false;
        }

        if (frameData == nullptr || dataSize == 0)
        {
            Logger::log(Logger::Level::ERROR_LEVEL, "Invalid frame data");
            return false;
        }

        // Verify expected frame size
        size_t expectedSize = m_config.width * m_config.height * 3; // RGB24
        if (dataSize != expectedSize)
        {
            Logger::log(Logger::Level::WARNING, 
                "Frame size mismatch: expected " + std::to_string(expectedSize) + 
                ", got " + std::to_string(dataSize));
        }

        return writeToPipe(frameData, dataSize);
    }

    bool FFmpegVideoWriter::writeToPipe(const void* data, size_t size)
    {
#ifdef _WIN32
        DWORD bytesWritten;
        BOOL success = WriteFile(m_stdinPipe, data, static_cast<DWORD>(size), &bytesWritten, nullptr);
        
        if (!success || bytesWritten != size)
        {
            Logger::log(Logger::Level::ERROR_LEVEL, "Failed to write to FFmpeg pipe");
            return false;
        }
        return true;

#else
        ssize_t bytesWritten = write(m_pipeFd, data, size);
        
        if (bytesWritten == -1)
        {
            Logger::log(Logger::Level::ERROR_LEVEL, "Failed to write to FFmpeg pipe: " + 
                std::string(strerror(errno)));
            return false;
        }
        
        if (static_cast<size_t>(bytesWritten) != size)
        {
            Logger::log(Logger::Level::WARNING, 
                "Partial write to FFmpeg pipe: " + std::to_string(bytesWritten) + 
                "/" + std::to_string(size) + " bytes");
            return false;
        }
        
        return true;
#endif
    }

    bool FFmpegVideoWriter::finalize()
    {
        if (!m_active)
        {
            return true;
        }

        Logger::log(Logger::Level::INFO, "Finalizing video encoding...");

        terminateFFmpegProcess();
        m_active = false;

        Logger::log(Logger::Level::INFO, "Video saved to: " + m_config.output);
        return true;
    }

    void FFmpegVideoWriter::terminateFFmpegProcess()
    {
#ifdef _WIN32
        if (m_stdinPipe != INVALID_HANDLE_VALUE)
        {
            CloseHandle(m_stdinPipe);
            m_stdinPipe = INVALID_HANDLE_VALUE;
        }

        if (m_processInfo.hProcess != nullptr)
        {
            // Wait for FFmpeg to finish encoding
            WaitForSingleObject(m_processInfo.hProcess, 5000); // 5 second timeout
            CloseHandle(m_processInfo.hProcess);
            CloseHandle(m_processInfo.hThread);
            ZeroMemory(&m_processInfo, sizeof(m_processInfo));
        }

#else
        if (m_pipeFd != -1)
        {
            close(m_pipeFd);
            m_pipeFd = -1;
        }

        if (m_processId != -1)
        {
            // Wait for FFmpeg to finish encoding
            int status;
            waitpid(m_processId, &status, 0);
            
            if (WIFEXITED(status))
            {
                int exitCode = WEXITSTATUS(status);
                if (exitCode != 0)
                {
                    Logger::log(Logger::Level::WARNING, 
                        "FFmpeg exited with code: " + std::to_string(exitCode));
                }
            }
            
            m_processId = -1;
        }
#endif
    }

    bool FFmpegVideoWriter::isActive() const
    {
        return m_active;
    }

} // namespace NanoRec

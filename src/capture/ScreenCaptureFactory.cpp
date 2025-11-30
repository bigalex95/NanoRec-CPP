/**
 * @file ScreenCaptureFactory.cpp
 * @brief Platform-specific screen capture factory implementation
 * @author NanoRec-CPP Team
 * @date 2025-11-30
 */

#include "capture/IScreenCapture.hpp"
#include "core/Logger.hpp"

#ifdef _WIN32
#include "capture/WindowsScreenCapture.hpp"
#elif __linux__
#include "capture/LinuxScreenCapture.hpp"
#else
#error "Unsupported platform"
#endif

namespace NanoRec
{

    std::unique_ptr<IScreenCapture> createScreenCapture()
    {
#ifdef _WIN32
        Logger::log(Logger::Level::INFO, "Creating Windows GDI screen capture instance");
        return std::make_unique<WindowsScreenCapture>();
#elif __linux__
        Logger::log(Logger::Level::INFO, "Creating Linux X11 screen capture instance");
        return std::make_unique<LinuxScreenCapture>();
#else
        Logger::log(Logger::Level::ERROR_LEVEL, "Unsupported platform for screen capture");
        return nullptr;
#endif
    }

} // namespace NanoRec

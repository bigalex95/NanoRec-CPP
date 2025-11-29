#include "core/Version.hpp"
#include "core/Logger.hpp"
#include <sstream>

// Include dependency headers
#include "imgui.h"

namespace NanoRec
{

    std::string Version::getString()
    {
        std::ostringstream oss;
        oss << MAJOR << "." << MINOR << "." << PATCH;
        return oss.str();
    }

    void Version::printInfo()
    {
        Logger::info("========================================");
        Logger::info("   NanoRec-CPP v" + getString());
        Logger::info("========================================");

        // C++ Standard
        std::ostringstream cppStd;
        cppStd << "C++ Standard: " << __cplusplus;
        Logger::info(cppStd.str());

        // ImGui Version
        std::string imguiVersion = "ImGui Version: ";
        imguiVersion += IMGUI_VERSION;
        Logger::info(imguiVersion);

        // MiniAudio
        Logger::info("MiniAudio: Header included successfully");
    }

} // namespace NanoRec

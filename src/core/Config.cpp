#include "core/Config.hpp"
#include "core/Logger.hpp"
#include <fstream>
#include <sstream>

namespace NanoRec
{

    Config &Config::getInstance()
    {
        static Config instance;
        return instance;
    }

    Config::Config()
    {
        resetToDefaults();
    }

    void Config::resetToDefaults()
    {
        // Video defaults
        m_videoConfig.width = 1920;
        m_videoConfig.height = 1080;
        m_videoConfig.fps = 60;
        m_videoConfig.bitrate = 5000;
        m_videoConfig.codec = "libx264";
        m_videoConfig.preset = "fast";

        // Audio defaults
        m_audioConfig.sampleRate = 44100;
        m_audioConfig.channels = 2;
        m_audioConfig.bitrate = 128;
        m_audioConfig.captureMicrophone = true;
        m_audioConfig.captureSystem = true;

        // App defaults
        m_appConfig.showPreview = true;
        m_appConfig.minimizeOnRecord = false;
        m_appConfig.outputDirectory = "./recordings";
        m_appConfig.outputFormat = "mp4";

        Logger::debug("Configuration reset to defaults");
    }

    bool Config::load(const std::string &filepath)
    {
        std::ifstream file(filepath);
        if (!file.is_open())
        {
            Logger::warning("Could not open config file: " + filepath);
            Logger::info("Using default configuration");
            return false;
        }

        // TODO: Implement proper config file parsing (JSON/INI format)
        // For now, just use defaults
        Logger::info("Configuration loaded from: " + filepath);
        file.close();
        return true;
    }

    bool Config::save(const std::string &filepath) const
    {
        std::ofstream file(filepath);
        if (!file.is_open())
        {
            Logger::error("Could not create config file: " + filepath);
            return false;
        }

        // TODO: Implement proper config file writing (JSON/INI format)
        // For now, just create a basic file
        file << "# NanoRec-CPP Configuration File\n";
        file << "# This is a placeholder - proper config format will be implemented\n";

        Logger::info("Configuration saved to: " + filepath);
        file.close();
        return true;
    }

} // namespace NanoRec

#pragma once

#include <string>
#include <cstdint>

namespace NanoRec
{

    /**
     * @brief Configuration management for NanoRec-CPP
     *
     * Handles application settings, video/audio parameters, and preferences.
     */
    class Config
    {
    public:
        // Video Settings
        struct VideoConfig
        {
            uint32_t width = 1920;
            uint32_t height = 1080;
            uint32_t fps = 60;
            uint32_t bitrate = 5000; // kbps
            std::string codec = "libx264";
            std::string preset = "fast"; // ultrafast, superfast, veryfast, faster, fast, medium, slow, slower, veryslow
        };

        // Audio Settings
        struct AudioConfig
        {
            uint32_t sampleRate = 44100;
            uint32_t channels = 2;  // Stereo
            uint32_t bitrate = 128; // kbps
            bool captureMicrophone = true;
            bool captureSystem = true;
        };

        // Application Settings
        struct AppConfig
        {
            bool showPreview = true;
            bool minimizeOnRecord = false;
            std::string outputDirectory = "./recordings";
            std::string outputFormat = "mp4";
        };

        /**
         * @brief Get the singleton instance
         */
        static Config &getInstance();

        // Delete copy constructor and assignment
        Config(const Config &) = delete;
        Config &operator=(const Config &) = delete;

        /**
         * @brief Load configuration from file
         * @param filepath Path to configuration file
         * @return true if loaded successfully
         */
        bool load(const std::string &filepath);

        /**
         * @brief Save configuration to file
         * @param filepath Path to configuration file
         * @return true if saved successfully
         */
        bool save(const std::string &filepath) const;

        /**
         * @brief Reset to default configuration
         */
        void resetToDefaults();

        // Getters
        VideoConfig &getVideoConfig() { return m_videoConfig; }
        AudioConfig &getAudioConfig() { return m_audioConfig; }
        AppConfig &getAppConfig() { return m_appConfig; }

        const VideoConfig &getVideoConfig() const { return m_videoConfig; }
        const AudioConfig &getAudioConfig() const { return m_audioConfig; }
        const AppConfig &getAppConfig() const { return m_appConfig; }

    private:
        Config();
        ~Config() = default;

        VideoConfig m_videoConfig;
        AudioConfig m_audioConfig;
        AppConfig m_appConfig;
    };

} // namespace NanoRec

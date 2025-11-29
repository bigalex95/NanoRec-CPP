#include "core/Logger.hpp"
#include <iomanip>
#include <ctime>

namespace NanoRec
{

    std::string Logger::getLevelString(Level level)
    {
        switch (level)
        {
        case Level::INFO:
            return "[INFO]";
        case Level::WARNING:
            return "[WARN]";
        case Level::ERROR:
            return "[ERROR]";
        case Level::DEBUG:
            return "[DEBUG]";
        default:
            return "[UNKNOWN]";
        }
    }

    void Logger::log(Level level, const std::string &message)
    {
        // Get current time
        auto now = std::time(nullptr);
        auto tm = *std::localtime(&now);

        std::cout << std::put_time(&tm, "%H:%M:%S") << " "
                  << getLevelString(level) << " "
                  << message << std::endl;
    }

    void Logger::info(const std::string &message)
    {
        log(Level::INFO, message);
    }

    void Logger::warning(const std::string &message)
    {
        log(Level::WARNING, message);
    }

    void Logger::error(const std::string &message)
    {
        log(Level::ERROR, message);
    }

    void Logger::debug(const std::string &message)
    {
        log(Level::DEBUG, message);
    }

} // namespace NanoRec

#pragma once

#include <iostream>
#include <string>
#include <sstream>

namespace NanoRec
{

    /**
     * @brief Simple logging utility for the application
     */
    class Logger
    {
    public:
        enum class Level
        {
            INFO,
            WARNING,
            ERROR_LEVEL,
            DEBUG
        };

        /**
         * @brief Log a message with the specified level
         */
        static void log(Level level, const std::string &message);

        /**
         * @brief Log an info message
         */
        static void info(const std::string &message);

        /**
         * @brief Log a warning message
         */
        static void warning(const std::string &message);

        /**
         * @brief Log an error message
         */
        static void error(const std::string &message);

        /**
         * @brief Log a debug message
         */
        static void debug(const std::string &message);

    private:
        static std::string getLevelString(Level level);
    };

} // namespace NanoRec

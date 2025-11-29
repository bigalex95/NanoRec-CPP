#pragma once

#include <string>

namespace NanoRec
{

    /**
     * @brief Version information for NanoRec-CPP
     */
    class Version
    {
    public:
        static constexpr int MAJOR = 0;
        static constexpr int MINOR = 1;
        static constexpr int PATCH = 0;

        /**
         * @brief Get version string in format "MAJOR.MINOR.PATCH"
         * @return Version string
         */
        static std::string getString();

        /**
         * @brief Print version and dependency information
         */
        static void printInfo();
    };

} // namespace NanoRec

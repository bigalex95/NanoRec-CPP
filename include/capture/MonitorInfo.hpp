#pragma once

#include <string>

namespace NanoRec
{

    /**
     * @struct MonitorInfo
     * @brief Information about a display monitor
     */
    struct MonitorInfo
    {
        int id;              ///< Monitor index (0, 1, 2, ...)
        std::string name;    ///< Monitor name ("Monitor 1", "HDMI-0", etc.)
        int x;               ///< X position in virtual desktop
        int y;               ///< Y position in virtual desktop
        int width;           ///< Monitor width in pixels
        int height;          ///< Monitor height in pixels
        bool isPrimary;      ///< True if this is the primary display

        MonitorInfo()
            : id(0), name("Unknown"), x(0), y(0), width(0), height(0), isPrimary(false)
        {
        }

        MonitorInfo(int id_, const std::string &name_, int x_, int y_, int w, int h, bool primary)
            : id(id_), name(name_), x(x_), y(y_), width(w), height(h), isPrimary(primary)
        {
        }

        /**
         * @brief Get display string for UI
         * @return Formatted string like "Monitor 1 (1920x1080)"
         */
        std::string getDisplayName() const
        {
            return name + " (" + std::to_string(width) + "x" + std::to_string(height) + ")";
        }
    };

} // namespace NanoRec

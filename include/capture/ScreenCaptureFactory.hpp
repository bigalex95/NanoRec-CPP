#pragma once

#include "capture/IScreenCapture.hpp"
#include <memory>

namespace NanoRec
{
    /**
     * @brief Create a platform-specific screen capture instance
     * @return Unique pointer to IScreenCapture implementation
     */
    std::unique_ptr<IScreenCapture> createScreenCapture();

} // namespace NanoRec

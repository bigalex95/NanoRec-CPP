#pragma once

#include <memory>
#include <string>

namespace NanoRec
{

    /**
     * @brief Main application class that manages the lifecycle of NanoRec-CPP
     */
    class Application
    {
    public:
        Application();
        ~Application();

        // Delete copy constructor and assignment operator
        Application(const Application &) = delete;
        Application &operator=(const Application &) = delete;

        /**
         * @brief Initialize the application and its dependencies
         * @return true if initialization succeeded, false otherwise
         */
        bool initialize();

        /**
         * @brief Run the main application loop
         * @return Exit code (0 for success)
         */
        int run();

        /**
         * @brief Cleanup and shutdown the application
         */
        void shutdown();

        /**
         * @brief Check if the application is running
         * @return true if running, false otherwise
         */
        bool isRunning() const;

    private:
        struct Impl;
        std::unique_ptr<Impl> m_impl;
    };

} // namespace NanoRec

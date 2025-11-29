#include "core/Application.hpp"
#include "core/Logger.hpp"
#include "core/Version.hpp"

namespace NanoRec
{

    // Private implementation (PIMPL pattern)
    struct Application::Impl
    {
        bool running = false;

        bool initializeDependencies()
        {
            Logger::info("Initializing dependencies...");

            // Check if dependencies are properly loaded
            // This will be expanded when we add GLFW, ImGui, etc.

            return true;
        }

        void mainLoop()
        {
            Logger::info("Application main loop started");
            // Main application loop will be implemented here
            // For now, just run once for testing
            running = false;
        }
    };

    Application::Application()
        : m_impl(std::make_unique<Impl>())
    {
    }

    Application::~Application()
    {
        shutdown();
    }

    bool Application::initialize()
    {
        Logger::info("Initializing NanoRec-CPP Application...");
        Version::printInfo();

        if (!m_impl->initializeDependencies())
        {
            Logger::error("Failed to initialize dependencies");
            return false;
        }

        m_impl->running = true;
        Logger::info("Application initialized successfully");
        return true;
    }

    int Application::run()
    {
        if (!m_impl->running)
        {
            Logger::error("Application not initialized. Call initialize() first.");
            return 1;
        }

        Logger::info("Starting application...");
        m_impl->mainLoop();

        return 0;
    }

    void Application::shutdown()
    {
        if (m_impl->running)
        {
            Logger::info("Shutting down application...");
            m_impl->running = false;
        }
    }

    bool Application::isRunning() const
    {
        return m_impl->running;
    }

} // namespace NanoRec

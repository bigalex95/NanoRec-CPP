#include "core/Application.hpp"
#include "core/Logger.hpp"

#include <exception>

/**
 * @brief Main entry point for NanoRec-CPP
 *
 * Initializes and runs the application with proper error handling
 * and resource management.
 */
int main(int argc, char *argv[])
{
    try
    {
        // Create application instance
        NanoRec::Application app;

        // Initialize the application
        if (!app.initialize())
        {
            NanoRec::Logger::error("Failed to initialize application");
            return 1;
        }

        // Run the application
        int exitCode = app.run();

        // Cleanup is handled by Application destructor
        return exitCode;
    }
    catch (const std::exception &e)
    {
        NanoRec::Logger::error(std::string("Unhandled exception: ") + e.what());
        return 1;
    }
    catch (...)
    {
        NanoRec::Logger::error("Unknown exception occurred");
        return 1;
    }
}
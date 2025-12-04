/**
 * @file test_window.cpp
 * @brief Test GLFW window creation and OpenGL context initialization
 *
 * This test verifies:
 * - GLFW library initialization
 * - Window creation with specified dimensions
 * - OpenGL context creation (version 3.3+)
 * - Basic rendering capability
 */

#include "core/Logger.hpp"

#include <GLFW/glfw3.h>
#include <iostream>
#include <string>

// GLFW error callback
static void errorCallback(int error, const char *description)
{
    std::cerr << "GLFW Error " << error << ": " << description << std::endl;
}

bool testGLFWInitialization()
{
    NanoRec::Logger::info("Test 1: GLFW Initialization");

    glfwSetErrorCallback(errorCallback);

    if (!glfwInit())
    {
        NanoRec::Logger::error("Failed to initialize GLFW");
        return false;
    }

    NanoRec::Logger::info("✓ GLFW initialized successfully");
    return true;
}

bool testWindowCreation()
{
    NanoRec::Logger::info("Test 2: Window Creation");

    // Configure GLFW for OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create window
    GLFWwindow *window = glfwCreateWindow(800, 600, "Test Window", nullptr, nullptr);
    if (!window)
    {
        NanoRec::Logger::error("Failed to create GLFW window");
        glfwTerminate();
        return false;
    }

    NanoRec::Logger::info("✓ Window created successfully (800x600)");

    // Make context current
    glfwMakeContextCurrent(window);

    // Cleanup
    glfwDestroyWindow(window);
    return true;
}

bool testOpenGLContext()
{
    NanoRec::Logger::info("Test 3: OpenGL Context");

    // Create window for context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(640, 480, "OpenGL Test", nullptr, nullptr);
    if (!window)
    {
        NanoRec::Logger::error("Failed to create window for OpenGL test");
        return false;
    }

    glfwMakeContextCurrent(window);

    // Get OpenGL version
    const char *version = (const char *)glGetString(GL_VERSION);
    const char *renderer = (const char *)glGetString(GL_RENDERER);
    const char *vendor = (const char *)glGetString(GL_VENDOR);

    if (!version)
    {
        NanoRec::Logger::error("Failed to get OpenGL version");
        glfwDestroyWindow(window);
        return false;
    }

    NanoRec::Logger::info("✓ OpenGL Context Created:");
    NanoRec::Logger::info("  Version:  " + std::string(version));
    NanoRec::Logger::info("  Renderer: " + std::string(renderer ? renderer : "Unknown"));
    NanoRec::Logger::info("  Vendor:   " + std::string(vendor ? vendor : "Unknown"));

    // Cleanup
    glfwDestroyWindow(window);
    return true;
}

bool testBasicRendering()
{
    NanoRec::Logger::info("Test 4: Basic Rendering");

    // Create window
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE); // Hidden window for testing

    GLFWwindow *window = glfwCreateWindow(640, 480, "Render Test", nullptr, nullptr);
    if (!window)
    {
        NanoRec::Logger::error("Failed to create window for render test");
        return false;
    }

    glfwMakeContextCurrent(window);

    // Test basic OpenGL rendering commands
    glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Check for OpenGL errors
    GLenum error = glGetError();
    if (error != GL_NO_ERROR)
    {
        NanoRec::Logger::error("OpenGL error during rendering: " + std::to_string(error));
        glfwDestroyWindow(window);
        return false;
    }

    NanoRec::Logger::info("✓ Basic rendering commands executed successfully");

    // Cleanup
    glfwDestroyWindow(window);
    return true;
}

int main()
{
    NanoRec::Logger::info("=== NanoRec-CPP Window Test Suite ===\n");

    bool allPassed = true;

    // Test 1: GLFW Initialization
    if (!testGLFWInitialization())
    {
        allPassed = false;
        NanoRec::Logger::error("✗ GLFW Initialization test failed\n");
    }
    else
    {
        std::cout << std::endl;
    }

    // Test 2: Window Creation
    if (!testWindowCreation())
    {
        allPassed = false;
        NanoRec::Logger::error("✗ Window Creation test failed\n");
    }
    else
    {
        std::cout << std::endl;
    }

    // Test 3: OpenGL Context
    if (!testOpenGLContext())
    {
        allPassed = false;
        NanoRec::Logger::error("✗ OpenGL Context test failed\n");
    }
    else
    {
        std::cout << std::endl;
    }

    // Test 4: Basic Rendering
    if (!testBasicRendering())
    {
        allPassed = false;
        NanoRec::Logger::error("✗ Basic Rendering test failed\n");
    }
    else
    {
        std::cout << std::endl;
    }

    // Cleanup GLFW
    glfwTerminate();

    // Print results
    NanoRec::Logger::info("=== Test Results ===");
    if (allPassed)
    {
        NanoRec::Logger::info("✓ All tests passed!");
        return 0;
    }
    else
    {
        NanoRec::Logger::error("✗ Some tests failed");
        return 1;
    }
}

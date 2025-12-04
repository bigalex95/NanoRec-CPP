/**
 * @file test_imgui_basic.cpp
 * @brief Test ImGui integration with GLFW and OpenGL
 *
 * This test verifies:
 * - ImGui context creation
 * - ImGui backends initialization (GLFW + OpenGL3)
 * - Basic UI rendering (buttons, text, windows)
 * - Button interaction
 */

#include "core/Logger.hpp"

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <iostream>

// GLFW error callback
static void errorCallback(int error, const char *description)
{
    std::cerr << "GLFW Error " << error << ": " << description << std::endl;
}

bool testImGuiContext()
{
    NanoRec::Logger::info("Test 1: ImGui Context Creation");

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGuiContext *ctx = ImGui::CreateContext();

    if (!ctx)
    {
        NanoRec::Logger::error("Failed to create ImGui context");
        return false;
    }

    NanoRec::Logger::info("✓ ImGui context created successfully");

    // Cleanup
    ImGui::DestroyContext(ctx);
    return true;
}

bool testImGuiBackends()
{
    NanoRec::Logger::info("Test 2: ImGui Backends Initialization");

    // Initialize GLFW
    glfwSetErrorCallback(errorCallback);
    if (!glfwInit())
    {
        NanoRec::Logger::error("Failed to initialize GLFW");
        return false;
    }

    // Create window
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE); // Hidden for testing

    GLFWwindow *window = glfwCreateWindow(640, 480, "ImGui Test", nullptr, nullptr);
    if (!window)
    {
        NanoRec::Logger::error("Failed to create window");
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);

    // Setup ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;

    // Initialize backends
    const char *glsl_version = "#version 330";
    if (!ImGui_ImplGlfw_InitForOpenGL(window, true))
    {
        NanoRec::Logger::error("Failed to initialize ImGui GLFW backend");
        ImGui::DestroyContext();
        glfwDestroyWindow(window);
        glfwTerminate();
        return false;
    }

    if (!ImGui_ImplOpenGL3_Init(glsl_version))
    {
        NanoRec::Logger::error("Failed to initialize ImGui OpenGL3 backend");
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        glfwDestroyWindow(window);
        glfwTerminate();
        return false;
    }

    NanoRec::Logger::info("✓ ImGui backends initialized successfully");

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return true;
}

bool testBasicUIRendering()
{
    NanoRec::Logger::info("Test 3: Basic UI Rendering");

    // Initialize GLFW
    glfwSetErrorCallback(errorCallback);
    if (!glfwInit())
    {
        NanoRec::Logger::error("Failed to initialize GLFW");
        return false;
    }

    // Create window
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    GLFWwindow *window = glfwCreateWindow(800, 600, "UI Render Test", nullptr, nullptr);
    if (!window)
    {
        NanoRec::Logger::error("Failed to create window");
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);

    // Setup ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsDark();

    const char *glsl_version = "#version 330";
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Test rendering a few frames
    for (int i = 0; i < 3; i++)
    {
        glfwPollEvents();

        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Create a test window
        ImGui::Begin("Test Window");
        ImGui::Text("This is a test");
        ImGui::Button("Test Button");
        ImGui::End();

        // Render
        ImGui::Render();

        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    NanoRec::Logger::info("✓ UI rendered successfully for 3 frames");

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return true;
}

bool testButtonInteraction()
{
    NanoRec::Logger::info("Test 4: Button Interaction");

    // Initialize GLFW
    glfwSetErrorCallback(errorCallback);
    if (!glfwInit())
    {
        NanoRec::Logger::error("Failed to initialize GLFW");
        return false;
    }

    // Create window
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

    GLFWwindow *window = glfwCreateWindow(800, 600, "Button Test", nullptr, nullptr);
    if (!window)
    {
        NanoRec::Logger::error("Failed to create window");
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);

    // Setup ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsDark();

    const char *glsl_version = "#version 330";
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Test button state
    bool buttonClicked = false;
    bool testPassed = true;

    // Render a frame with button
    glfwPollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Button Test");
    if (ImGui::Button("Click Me"))
    {
        buttonClicked = true;
    }
    ImGui::End();

    ImGui::Render();

    // Button shouldn't be clicked without mouse input
    if (buttonClicked)
    {
        NanoRec::Logger::error("Button clicked without user input (unexpected)");
        testPassed = false;
    }
    else
    {
        NanoRec::Logger::info("✓ Button state correctly initialized");
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return testPassed;
}

int main()
{
    NanoRec::Logger::info("=== NanoRec-CPP ImGui Test Suite ===\n");

    bool allPassed = true;

    // Test 1: ImGui Context
    if (!testImGuiContext())
    {
        allPassed = false;
        NanoRec::Logger::error("✗ ImGui Context test failed\n");
    }
    else
    {
        std::cout << std::endl;
    }

    // Test 2: ImGui Backends
    if (!testImGuiBackends())
    {
        allPassed = false;
        NanoRec::Logger::error("✗ ImGui Backends test failed\n");
    }
    else
    {
        std::cout << std::endl;
    }

    // Test 3: Basic UI Rendering
    if (!testBasicUIRendering())
    {
        allPassed = false;
        NanoRec::Logger::error("✗ Basic UI Rendering test failed\n");
    }
    else
    {
        std::cout << std::endl;
    }

    // Test 4: Button Interaction
    if (!testButtonInteraction())
    {
        allPassed = false;
        NanoRec::Logger::error("✗ Button Interaction test failed\n");
    }
    else
    {
        std::cout << std::endl;
    }

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

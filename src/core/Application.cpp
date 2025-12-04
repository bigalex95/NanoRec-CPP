#include "core/Application.hpp"
#include "core/Logger.hpp"
#include "core/Version.hpp"

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

namespace NanoRec
{
    // GLFW error callback
    static void glfwErrorCallback(int error, const char *description)
    {
        Logger::error("GLFW Error " + std::to_string(error) + ": " + description);
    }

    // Private implementation (PIMPL pattern)
    struct Application::Impl
    {
        bool running = false;
        GLFWwindow *window = nullptr;
        int windowWidth = 1280;
        int windowHeight = 720;

        // UI State
        bool isRecording = false;
        std::string statusText = "Ready";

        bool initializeGLFW()
        {
            Logger::info("Initializing GLFW...");

            // Set error callback before initialization
            glfwSetErrorCallback(glfwErrorCallback);

            // Initialize GLFW
            if (!glfwInit())
            {
                Logger::error("Failed to initialize GLFW");
                return false;
            }

            // Configure GLFW for OpenGL 3.3 Core Profile
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

            Logger::info("GLFW initialized successfully");
            return true;
        }

        bool createWindow()
        {
            Logger::info("Creating window (" + std::to_string(windowWidth) + "x" +
                         std::to_string(windowHeight) + ")...");

            // Create window
            window = glfwCreateWindow(windowWidth, windowHeight, "NanoRec-CPP", nullptr, nullptr);
            if (!window)
            {
                Logger::error("Failed to create GLFW window");
                glfwTerminate();
                return false;
            }

            // Make OpenGL context current
            glfwMakeContextCurrent(window);

            // Enable vsync
            glfwSwapInterval(1);

            Logger::info("Window created successfully");
            return true;
        }

        bool initializeOpenGL()
        {
            Logger::info("Initializing OpenGL...");

            // Get OpenGL version
            const char *version = (const char *)glGetString(GL_VERSION);
            const char *renderer = (const char *)glGetString(GL_RENDERER);

            if (version)
            {
                Logger::info("OpenGL Version: " + std::string(version));
            }
            if (renderer)
            {
                Logger::info("OpenGL Renderer: " + std::string(renderer));
            }

            // Setup viewport
            int width, height;
            glfwGetFramebufferSize(window, &width, &height);
            glViewport(0, 0, width, height);

            Logger::info("OpenGL initialized successfully");
            return true;
        }

        bool initializeImGui()
        {
            Logger::info("Initializing ImGui...");

            // Setup Dear ImGui context
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGuiIO &io = ImGui::GetIO();
            (void)io;
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls

            // Setup Dear ImGui style
            ImGui::StyleColorsDark();

            // Setup Platform/Renderer backends
            const char *glsl_version = "#version 330";
            if (!ImGui_ImplGlfw_InitForOpenGL(window, true))
            {
                Logger::error("Failed to initialize ImGui GLFW backend");
                return false;
            }

            if (!ImGui_ImplOpenGL3_Init(glsl_version))
            {
                Logger::error("Failed to initialize ImGui OpenGL3 backend");
                return false;
            }

            Logger::info("ImGui initialized successfully");
            return true;
        }

        bool initializeDependencies()
        {
            Logger::info("Initializing dependencies...");

            if (!initializeGLFW())
            {
                return false;
            }

            if (!createWindow())
            {
                return false;
            }

            if (!initializeOpenGL())
            {
                return false;
            }

            if (!initializeImGui())
            {
                return false;
            }

            return true;
        }

        void renderUI()
        {
            // Start the Dear ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            // Create main control window
            ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowSize(ImVec2(300, 150), ImGuiCond_FirstUseEver);

            ImGui::Begin("NanoRec Controls", nullptr, ImGuiWindowFlags_NoCollapse);

            // Status display
            ImGui::Text("Status: %s", statusText.c_str());
            ImGui::Separator();

            // Start/Stop buttons
            if (!isRecording)
            {
                if (ImGui::Button("Start Recording", ImVec2(280, 30)))
                {
                    isRecording = true;
                    statusText = "Recording...";
                    Logger::info("Recording started");
                }
            }
            else
            {
                if (ImGui::Button("Stop Recording", ImVec2(280, 30)))
                {
                    isRecording = false;
                    statusText = "Stopped";
                    Logger::info("Recording stopped");
                }
            }

            ImGui::Spacing();

            // Quit button
            if (ImGui::Button("Quit", ImVec2(280, 30)))
            {
                running = false;
                Logger::info("Quit requested");
            }

            ImGui::End();

            // Render ImGui
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }

        void mainLoop()
        {
            Logger::info("Application main loop started");

            // Main event loop
            while (!glfwWindowShouldClose(window) && running)
            {
                // Poll events
                glfwPollEvents();

                // Clear screen with a dark gray color
                glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT);

                // Render UI
                renderUI();

                // Swap buffers
                glfwSwapBuffers(window);
            }

            Logger::info("Application main loop ended");
        }

        void cleanup()
        {
            // Cleanup ImGui
            if (window)
            {
                Logger::info("Shutting down ImGui...");
                ImGui_ImplOpenGL3_Shutdown();
                ImGui_ImplGlfw_Shutdown();
                ImGui::DestroyContext();
            }

            if (window)
            {
                Logger::info("Destroying window...");
                glfwDestroyWindow(window);
                window = nullptr;
            }

            Logger::info("Terminating GLFW...");
            glfwTerminate();
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
            m_impl->cleanup();
        }
    }

    bool Application::isRunning() const
    {
        return m_impl->running;
    }

} // namespace NanoRec

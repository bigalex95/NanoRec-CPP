cmake_minimum_required(VERSION 3.10)
project(NanoRec-CPP VERSION 0.1.0 LANGUAGES CXX)

# --- 1. Project Settings ---
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Useful for clangd/VSCode

# --- 2. Dependencies (GLFW) ---
# Download GLFW automatically
include(FetchContent)
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw
    GIT_TAG 3.3.8
)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

# --- 3. Source Files ---
# Main Application Sources
set(SOURCES
    src/main.cpp
    src/core/Application.cpp
    src/core/Version.cpp
    src/core/Logger.cpp
    src/core/Config.cpp
    src/capture/ScreenCaptureFactory.cpp
)

# Platform-specific capture sources
if(WIN32)
    list(APPEND SOURCES src/capture/WindowsScreenCapture.cpp)
elseif(UNIX AND NOT APPLE)
    list(APPEND SOURCES src/capture/LinuxScreenCapture.cpp)
endif()

# ImGui Sources
list(APPEND SOURCES
    vendor/imgui/imgui.cpp
    vendor/imgui/imgui_draw.cpp
    vendor/imgui/imgui_tables.cpp
    vendor/imgui/imgui_widgets.cpp
    vendor/imgui/imgui_demo.cpp
    vendor/imgui/backends/imgui_impl_glfw.cpp
    vendor/imgui/backends/imgui_impl_opengl3.cpp
)

# --- 4. Executable Definition ---
add_executable(${PROJECT_NAME} ${SOURCES})

# Set output directory for main executable
# Handle both single-config (Unix Makefiles) and multi-config (Visual Studio) generators
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin/Debug
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/Release
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/bin/RelWithDebInfo
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_BINARY_DIR}/bin/MinSizeRel
)

# --- 5. Include Directories ---
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/miniaudio
)

# --- 6. Linking ---
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)

# --- 7. Platform Specifics ---
if(WIN32)
    # Windows System Libraries
    target_link_libraries(${PROJECT_NAME} PRIVATE Gdi32 User32 Shell32)
elseif(UNIX AND NOT APPLE)
    # Linux (X11) Libraries
    target_link_libraries(${PROJECT_NAME} PRIVATE X11 pthread dl)
endif()

# --- 8. Tests ---
option(BUILD_TESTS "Build test executables" ON)

if(BUILD_TESTS)
    # Screen Capture Test
    add_executable(test_capture
        tests/test_capture.cpp
        src/core/Logger.cpp
        src/capture/ScreenCaptureFactory.cpp
    )

    # Add platform-specific capture implementation
    if(WIN32)
        target_sources(test_capture PRIVATE src/capture/WindowsScreenCapture.cpp)
    elseif(UNIX AND NOT APPLE)
        target_sources(test_capture PRIVATE src/capture/LinuxScreenCapture.cpp)
    endif()

    target_include_directories(test_capture PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

    # Link platform-specific libraries
    if(WIN32)
        target_link_libraries(test_capture PRIVATE Gdi32 User32)
    elseif(UNIX AND NOT APPLE)
        target_link_libraries(test_capture PRIVATE X11)
    endif()

    # Set output directory (handle multi-config generators)
    set_target_properties(test_capture PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/tests
        RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin/tests/Debug
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/tests/Release
        RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/bin/tests/RelWithDebInfo
        RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_BINARY_DIR}/bin/tests/MinSizeRel
    )

    message(STATUS "Tests enabled: test_capture -> bin/tests/")
endif()

message(STATUS "Build configured for: ${CMAKE_SYSTEM_NAME}")
message(STATUS "Executables will be in: ${CMAKE_BINARY_DIR}/bin/")

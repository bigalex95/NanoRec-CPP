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
)

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

message(STATUS "Build configured for: ${CMAKE_SYSTEM_NAME}")

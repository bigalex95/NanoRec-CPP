# Developer Quick Reference

## 🚀 Quick Start

### Build and Run

```bash
./scripts/build.sh      # Compile the project
./scripts/run.sh        # Run the executable
./scripts/setup.sh      # Download dependencies (if needed)
```

---

## 📁 Project Structure Quick Reference

```
include/core/          → Public headers (use these in your code)
src/core/              → Implementation files (internal)
vendor/                → Third-party libraries
build/                 → Compiled binaries and build files
```

---

## 💡 Common Code Snippets

### Using the Logger

```cpp
#include "core/Logger.hpp"

// Log different levels
NanoRec::Logger::info("Application started");
NanoRec::Logger::warning("Low memory detected");
NanoRec::Logger::error("Failed to open file");
NanoRec::Logger::debug("Variable x = " + std::to_string(x));
```

### Using the Config System

```cpp
#include "core/Config.hpp"

// Get singleton instance
auto& config = NanoRec::Config::getInstance();

// Access video settings
config.getVideoConfig().width = 1920;
config.getVideoConfig().height = 1080;
config.getVideoConfig().fps = 60;

// Access audio settings
config.getAudioConfig().sampleRate = 48000;
config.getAudioConfig().channels = 2;

// Access app settings
config.getAppConfig().showPreview = true;
config.getAppConfig().outputDirectory = "./my_recordings";

// Save/Load (placeholder - implement when needed)
config.save("config.ini");
config.load("config.ini");
```

### Creating a New Component

```cpp
// In include/core/MyComponent.hpp
#pragma once

namespace NanoRec {

class MyComponent {
public:
    MyComponent();
    ~MyComponent();

    bool initialize();
    void update();
    void shutdown();

private:
    // Private members
};

} // namespace NanoRec

// In src/core/MyComponent.cpp
#include "core/MyComponent.hpp"
#include "core/Logger.hpp"

namespace NanoRec {

MyComponent::MyComponent() {
    Logger::debug("MyComponent created");
}

bool MyComponent::initialize() {
    Logger::info("Initializing MyComponent...");
    // Implementation
    return true;
}

} // namespace NanoRec
```

---

## 🔧 CMake Integration

### Adding a New Source File

Edit `CMakeLists.txt`:

```cmake
# Find this section:
set(SOURCES
    src/main.cpp
    src/core/Application.cpp
    src/core/Version.cpp
    src/core/Logger.cpp
    src/core/Config.cpp
    # Add your new file here:
    src/core/MyComponent.cpp
)
```

---

## 🎨 Coding Style Guidelines

### Naming Conventions

```cpp
// Classes: PascalCase
class ScreenCapture { };

// Methods: camelCase
void captureFrame() { }

// Private members: m_ prefix
class MyClass {
    int m_counter;
    std::string m_name;
};

// Constants: UPPER_CASE
constexpr int MAX_BUFFER_SIZE = 1024;

// Namespaces: PascalCase
namespace NanoRec { }
```

### Header Guards

```cpp
#pragma once  // Preferred (supported by all modern compilers)

// OR traditional:
#ifndef NANOREC_MYFILE_HPP
#define NANOREC_MYFILE_HPP
// ... content ...
#endif
```

### Include Order

```cpp
// 1. Corresponding header (for .cpp files)
#include "core/MyClass.hpp"

// 2. Project headers
#include "core/Logger.hpp"
#include "core/Config.hpp"

// 3. Third-party headers
#include "imgui.h"
#include "miniaudio.h"

// 4. Standard library headers
#include <iostream>
#include <vector>
#include <memory>
```

---

## 🐛 Debugging Tips

### Enable Debug Logging

```cpp
// Use debug logs liberally during development
Logger::debug("Entering function X");
Logger::debug("Variable value: " + std::to_string(value));
```

### Check Build Errors

```bash
./scripts/build.sh 2>&1 | grep error    # Filter errors
./scripts/build.sh 2>&1 | grep warning  # Filter warnings
```

### Run with Verbose Output

```bash
# The logger already timestamps everything
./scripts/run.sh
```

---

## 📊 Testing Checklist

Before committing code:

- [ ] Code compiles without errors
- [ ] No compiler warnings
- [ ] Follows project naming conventions
- [ ] Added appropriate logging
- [ ] Updated CMakeLists.txt if needed
- [ ] Added header comments
- [ ] Tested basic functionality

---

## 🎯 Next Development Steps

Based on `docs/tasks.md`, the priority order is:

1. **Screen Capture Module** (`IScreenCapture` interface)

   - Create `include/capture/IScreenCapture.hpp`
   - Implement platform-specific versions

2. **Video Encoder** (FFmpeg integration)

   - Create `include/video/VideoWriter.hpp`
   - Implement subprocess management

3. **Audio Capture** (MiniAudio)

   - Create `include/audio/AudioCapture.hpp`
   - Integrate with Config system

4. **UI System** (ImGui + GLFW)
   - Create `include/ui/UIManager.hpp`
   - Integrate with Application class

---

## 🔍 Useful VS Code Tasks

If using VS Code, you can add these to `.vscode/tasks.json`:

```json
{
  "version": "2.0.0",
  "tasks": [
    {
      "label": "Build NanoRec",
      "type": "shell",
      "command": "./scripts/build.sh",
      "group": {
        "kind": "build",
        "isDefault": true
      }
    },
    {
      "label": "Run NanoRec",
      "type": "shell",
      "command": "./scripts/run.sh",
      "group": {
        "kind": "test",
        "isDefault": true
      }
    }
  ]
}
```

Or add custom keybindings in `.vscode/keybindings.json`:

```json
[
  {
    "key": "ctrl+shift+b",
    "command": "workbench.action.tasks.build"
  },
  {
    "key": "f5",
    "command": "workbench.action.tasks.test"
  },
  {
    "key": "ctrl+alt+r",
    "command": "workbench.action.tasks.runTask",
    "args": "Run NanoRec"
  }
]
```

Then use:

- `Ctrl+Shift+B` to build
- `F5` or `Ctrl+Alt+R` to run

---

## 📚 Resources

- **C++ Reference**: https://en.cppreference.com/
- **CMake Docs**: https://cmake.org/documentation/
- **ImGui Wiki**: https://github.com/ocornut/imgui/wiki
- **MiniAudio**: https://miniaud.io/
- **GLFW Docs**: https://www.glfw.org/documentation.html

---

## 🆘 Troubleshooting

### Build fails with "No such file or directory"

```bash
# Re-run setup to ensure all dependencies are present
./scripts/setup.sh
```

### Linker errors

```bash
# Clean and rebuild
rm -rf build/
./scripts/build.sh
```

### ImGui/GLFW not found

```bash
# Check vendor directory
ls vendor/imgui/
ls build/_deps/glfw-src/

# Re-download if missing
./scripts/setup.sh
```

---

## ✨ Pro Tips

1. **Use the convenience header**: `#include "NanoRec.hpp"` includes all core components

2. **Logger is your friend**: Add logs liberally during development

3. **Config is a singleton**: Access it anywhere with `Config::getInstance()`

4. **Follow RAII**: Let destructors clean up resources automatically

5. **Use smart pointers**: `std::unique_ptr`, `std::shared_ptr` instead of raw pointers

6. **Keep namespaces consistent**: Everything should be in `NanoRec::`

---

**Happy coding! 🚀**

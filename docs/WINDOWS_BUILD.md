# Windows Build & Deployment

## Quick Fix for "VCRUNTIME140_1.dll not found" Error

If you're getting this error on Windows, you have two options:

### Option 1: Install Visual C++ Redistributable (Quick)
Download and install: https://aka.ms/vs/17/release/vc_redist.x64.exe

### Option 2: Build with Static Linking (Better for Distribution)
The project is now configured to use static linking by default on Windows, which means the executable won't need any DLL files.

**Rebuild the project:**
```bash
# Clean and reconfigure
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build build --config Release
```

The resulting executable will be standalone with no external DLL dependencies.

## Building on Windows

### Prerequisites
- Visual Studio 2019 or later (with C++ Desktop Development workload)
- CMake 3.10 or later
- Git

### Build Steps

```bash
# Clone the repository
git clone https://github.com/yourusername/NanoRec-CPP.git
cd NanoRec-CPP

# Initialize submodules
git submodule update --init --recursive

# Configure (uses static runtime by default)
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build build --config Release

# Run
.\build\bin\Release\NanoRec-CPP.exe
```

### Build Configurations

**Release (Recommended for distribution):**
```bash
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

**Debug:**
```bash
cmake -B build -S . -DCMAKE_BUILD_TYPE=Debug
cmake --build build --config Debug
```

## Distribution

The executable in `build/bin/Release/NanoRec-CPP.exe` is now **standalone** and can be distributed without any additional DLLs.

### Creating a Release Package

1. Build in Release mode
2. Copy `build/bin/Release/NanoRec-CPP.exe` to a distribution folder
3. (Optional) Add a README and any configuration files
4. Zip the folder

No need to bundle Visual C++ Redistributable or any DLLs!

## Troubleshooting

### "VCRUNTIME140_1.dll not found" still appears
- Make sure you rebuilt the project after updating CMakeLists.txt
- Verify you're using the newly built executable, not an old one
- Check that CMake configuration shows: "Windows: Using static runtime library"

### Build errors related to runtime library
If you get linker errors, ensure all dependencies (GLFW, ImGui) are also using the same runtime library. The current CMakeLists.txt handles this automatically.

## Technical Details

The project uses `CMAKE_MSVC_RUNTIME_LIBRARY` to statically link the Microsoft C++ runtime:
- Release builds use `/MT` (MultiThreaded)
- Debug builds use `/MTd` (MultiThreadedDebug)

This eliminates the need for:
- `VCRUNTIME140.dll`
- `VCRUNTIME140_1.dll`
- `MSVCP140.dll`

The trade-off is a slightly larger executable size (~200-500KB increase), but this is negligible compared to the convenience of a standalone executable.

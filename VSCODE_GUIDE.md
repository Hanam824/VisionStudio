# Running & Debugging VisionStudio in VS Code

This repository is fully configured for **cross-platform development** with Visual Studio Code on **Windows**, **Linux**, and **macOS**.

---

## 1. Prerequisites

### All Platforms
- [Visual Studio Code](https://code.visualstudio.com/)
- [CMake](https://cmake.org/) ≥ 3.21
- Git (with submodules: `git clone --recurse-submodules`)

### Windows
- **Visual Studio 2022** with the "Desktop development with C++" workload
  - Provides MSVC compiler and the `cppvsdbg` debugger
- vcpkg bootstraps automatically from `third-party/vcpkg/`

### Linux (Ubuntu/Debian)
- Build essentials and Ninja:
  ```bash
  sudo apt-get install build-essential ninja-build cmake pkg-config gdb \
    libgl1-mesa-dev libglu1-mesa-dev libegl1-mesa-dev libxkbcommon-x11-dev \
    libx11-xcb-dev libxcb1-dev libxcb-cursor-dev libxcb-keysyms1-dev \
    libxcb-image0-dev libxcb-icccm4-dev libxcb-render-util0-dev \
    libxcb-xinerama0-dev libxcb-shape0-dev libxcb-xfixes0-dev \
    libxcb-randr0-dev libxcb-sync-dev libxcb-shm0-dev libxcb-render0-dev \
    libxcb-glx0-dev libx11-dev libxft-dev libxext-dev libwayland-dev \
    libxrender-dev libharfbuzz-dev libfontconfig1-dev libfreetype6-dev \
    libdbus-1-dev libxkbcommon-dev
  ```
  > **Note**: `gdb` is required for the debugger.

### macOS
- **Xcode** (or Xcode Command Line Tools): `xcode-select --install`
- Homebrew packages:
  ```bash
  brew install ninja cmake pkg-config autoconf autoconf-archive automake libtool libomp
  ```
  > LLDB ships with Xcode and is used as the macOS debugger.

---

## 2. Recommended VS Code Extensions

When opening this folder in VS Code, install the recommended extensions when prompted (or search in the Extensions view `Ctrl+Shift+X` / `Cmd+Shift+X`):

| Extension | ID | Purpose |
|---|---|---|
| **C/C++** | `ms-vscode.cpptools` | IntelliSense + native debugger (`cppvsdbg` / `cppdbg`) |
| **CMake Tools** | `ms-vscode.cmake-tools` | Preset selection, building, target execution |
| **CMake** | `twxs.cmake` | Syntax highlighting for `CMakeLists.txt` |
| **C/C++ Themes** | `ms-vscode.cpptools-themes` | Enhanced debug visualization |

---

## 3. First-Time Setup

After cloning with submodules, bootstrap vcpkg:

```bash
# Windows (PowerShell)
.\third-party\vcpkg\bootstrap-vcpkg.bat -disableMetrics

# Linux / macOS
./third-party/vcpkg/bootstrap-vcpkg.sh -disableMetrics
```

Then configure:

```bash
# Windows
cmake --preset windows-x64

# Linux
cmake --preset linux-x64-debug      # Debug
cmake --preset linux-deb-x64        # Release (.deb)

# macOS (Apple Silicon)
cmake --preset macos-arm64

# macOS (Intel)
cmake --preset macos-x64
```

> **Tip**: With `cmake.configureOnOpen` enabled in settings, VS Code will automatically configure when you open the folder.

---

## 4. Building the Project

### Option A: Keyboard Shortcut (`Ctrl+Shift+B`)
Press **`Ctrl+Shift+B`** (or **`Cmd+Shift+B`** on macOS) and select the build task for your platform:

| Task | Platform | Configuration |
|---|---|---|
| `Build: Debug (Windows)` | Windows | Debug |
| `Build: Release (Windows)` | Windows | Release (default) |
| `Build: Debug (Linux)` | Linux | Debug |
| `Build: Release (Linux DEB)` | Linux | Release |
| `Build: Debug (macOS)` | macOS | Debug (Apple Silicon) |
| `Build: Release (macOS)` | macOS | Release (Apple Silicon) |

### Option B: Terminal
```bash
# Windows
cmake --build --preset windows-x64-debug --parallel    # Debug
cmake --build --preset windows-x64 --parallel          # Release

# Linux
cmake --build --preset linux-x64-debug --parallel      # Debug
cmake --build --preset linux-deb-x64 --parallel        # Release

# macOS
cmake --build --preset macos-arm64-debug --parallel     # Debug
cmake --build --preset macos-arm64 --parallel           # Release
```

---

## 5. Running & Debugging (`F5`)

Press **F5** to launch the debugger. Select the appropriate configuration from the Debug panel (`Ctrl+Shift+D` / `Cmd+Shift+D`):

### Windows (MSVC Debugger — `cppvsdbg`)
| Configuration | Description |
|---|---|
| `Debug VisionStudio (Windows)` | Launches Debug build with breakpoints and full symbol support |
| `Release VisionStudio (Windows)` | Launches Release build |
| `Debug VisionTests (Windows)` | Debug the GoogleTest test suite |

### Linux (GDB — `cppdbg`)
| Configuration | Description |
|---|---|
| `Debug VisionStudio (Linux)` | Launches Debug build under GDB |
| `Debug VisionTests (Linux)` | Debug tests under GDB |

### macOS (LLDB — `cppdbg`)
| Configuration | Description |
|---|---|
| `Debug VisionStudio (macOS)` | Launches Debug build under LLDB |
| `Debug VisionTests (macOS)` | Debug tests under LLDB |

> **Note**: Each configuration automatically builds before launching (via `preLaunchTask`). You don't need to build manually first.

---

## 6. Running Unit Tests

### Option A: VS Code Task
Open **Terminal > Run Task... > `Run Tests (CTest)`** and pick the build directory for your platform.

### Option B: Terminal
```bash
# Windows
ctest --test-dir build/windows-x64 --output-on-failure

# Linux
ctest --test-dir build/linux-x64-debug --output-on-failure

# macOS
ctest --test-dir build/macos-arm64 --output-on-failure
```

---

## 7. macOS: Intel vs Apple Silicon

If you're on an **Intel Mac**, use the `macos-x64` presets instead of `macos-arm64`:

- Configure: `cmake --preset macos-x64`
- Build Debug: `cmake --build --preset macos-x64-debug --parallel`
- In the Debug panel, you can duplicate the macOS launch config and change the `preLaunchTask` to use the Intel build task.

---

## 8. Output Directories

All platforms share the same output layout:

| Directory | Contents |
|---|---|
| `bin/Debug/` | Debug executables + DLLs/shared libs |
| `bin/Release/` | Release executables + DLLs/shared libs |
| `lib/Debug/` | Debug import/static libraries |
| `lib/Release/` | Release import/static libraries |
| `build/<preset>/` | CMake build trees (intermediate files) |

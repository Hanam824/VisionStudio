# Running & Debugging VisionStudio in VS Code

This repository is fully pre-configured for **Visual Studio Code** development on Windows without needing the heavy Visual Studio IDE interface.

---

## 1. Recommended VS Code Extensions

When opening this folder in VS Code, install the recommended extensions when prompted (or search in the Extensions view `Ctrl+Shift+X`):
- **C/C++** (`ms-vscode.cpptools`) – IntelliSense and native `cppvsdbg` debugger
- **CMake Tools** (`ms-vscode.cmake-tools`) – Seamless CMake preset selection, building, and target execution
- **CMake** (`twxs.cmake`) – Syntax highlighting for `CMakeLists.txt`

---

## 2. Building the Project

### Option A: Using VS Code Build Tasks (`Ctrl+Shift+B`)
Press **`Ctrl+Shift+B`** (or go to **Terminal > Run Build Task...**) and select:
- **`CMake: Build VisionStudio (Release)`** (default)
- **`CMake: Build VisionStudio (Debug)`**

### Option B: Using CMake Tools Status Bar
In the bottom VS Code status bar:
1. Click **Configure Preset** and select **`Windows x64 (Release)`** or **`Windows x64 (Debug)`**.
2. Click **Build Preset** or press **F7**.

---

## 3. Running & Debugging (`F5` or `Ctrl+F5`)

Press **F5** (Debug) or **Ctrl+F5** (Run Without Debugging) at any time.

Available Launch Configurations (in the Debug panel `Ctrl+Shift+D`):
1. **`Launch VisionStudio (Release)`** – Runs optimized `VisionStudio.exe` with GUI.
2. **`Launch VisionStudio (Debug)`** – Runs `VisionStudio.exe` with symbol loading and breakpoint support.
3. **`Debug VisionTests`** – Launches the GoogleTest unit & integration test suite (`VisionTests.exe`) under the debugger.

---

## 4. Running Unit Tests

Run the full automated test suite directly from VS Code:
- Open **Terminal > Run Task... > `Run Tests (VisionTests)`**
- Or run in the terminal:
  ```pwsh
  .\bin\Release\VisionTests.exe
  ```

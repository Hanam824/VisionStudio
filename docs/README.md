# Vision Studio — Documentation

Welcome to the **Vision Studio** documentation. This folder contains all user-facing guides and references for the project.

## 📚 Contents

| Document | Description | Audience |
|----------|-------------|----------|
| [Getting Started](getting_started.md) | Install → Build → Run in 5 minutes | Beginners |
| [Build Guide](build_guide.md) | Detailed build instructions per platform | All developers |
| [Architecture](architecture.md) | System design, module boundaries, data flow | Contributors |
| [Contributing](contributing.md) | Code style, PR process, naming conventions | Contributors |
| [FAQ](faq.md) | Common errors & troubleshooting | Everyone |

## 🔗 Related

- **[MASTER_PLAN.md](../plans/MASTER_PLAN.md)** — Full project blueprint & technical spec
- **[Implementation Plan](../plans/implementation_plan.md)** — Current sprint breakdown
- **[Walkthrough](../plans/walkthrough.md)** — What was built & how

## Quick Links

- **Primary IDE**: Visual Studio Community 2022
- **Language**: C++17
- **UI Framework**: Qt 5.15
- **Image Processing**: OpenCV 4
- **AI Inference**: ncnn / ONNX Runtime
- **Build System**: CMake + vcpkg (bundled as submodule)

---

## 🛠️ Setup & Prerequisites

Platform-specific setup guides to prepare your environment for building Vision Studio:

- **[Linux Setup Guide](setup_linux.md)** (Includes Ubuntu CMake upgrade guide)
- *Windows Setup Guide (Coming Soon)*
- *macOS Setup Guide (Coming Soon)*

---

## 🚀 Building the Project

Once your setup is complete, you can build the project. For detailed instructions, refer to the **[Build Guide](build_guide.md)**.

### Quick Start

1. **Initialize git submodules** (if you didn't clone with `--recursive`):
   ```bash
   git submodule update --init --recursive
   ```

2. **Run the build script:**
   - **Linux / macOS:** `./run_build.sh`
   - **Windows:** `run_build.bat`

> [!NOTE]
> **VCPKG Cache:** The build scripts will automatically create a `cache/` directory in the project root. This caches compiled third-party libraries so subsequent rebuilds are instantaneous. If you need to free up disk space, you can safely delete the `cache/` directory at any time.

The script will automatically configure the correct CMake preset for your operating system, install dependencies via `vcpkg`, and compile the project into the `build/` directory.

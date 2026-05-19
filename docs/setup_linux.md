# Linux Setup & Prerequisites

This guide covers setting up your Linux environment (specifically Ubuntu) to build Vision Studio.

## Prerequisites

- **CMake 3.25** or higher (Required for `CMakePresets.json` v6 schema)
- A modern C++ compiler (GCC or Clang)
- Git (for submodules and vcpkg)

---

## 🐧 Ubuntu 22.04 Setup (For Beginners)

### 1. Install System Dependencies

You will need a C++ compiler, Ninja build system, and basic tools required by `vcpkg` to build libraries:

```bash
sudo apt update
sudo apt install build-essential ninja-build pkg-config curl zip unzip tar bison \
  autoconf autoconf-archive automake libtool libgl1-mesa-dev libegl1-mesa-dev \
  libxkbcommon-x11-dev libxcb-xinerama0-dev libxcb-shape0-dev libxcb-xfixes0-dev \
  libxcb-util-dev libxcb-util0-dev libxcb-icccm4-dev libxcb-image0-dev libxcb-keysyms1-dev \
  libxcb-randr0-dev libxcb-render-util0-dev libxcb-sync-dev libx11-dev libxft-dev \
  libxext-dev libwayland-dev libxrender-dev libx11-xcb-dev libxkbcommon-dev \
  python3-setuptools python3-pip python3-venv gperf flex libasound2-dev \
  libharfbuzz-dev libpango1.0-dev libatk1.0-dev libgdk-pixbuf2.0-dev libglib2.0-dev \
  libgtk-3-dev libipc-run-perl libltdl-dev
```

### 2. Upgrade CMake

Ubuntu 22.04 ships with CMake `3.22.1` by default, which is **too old** to configure this project (requires `>= 3.25`). You must upgrade your CMake version to build Vision Studio.

### Option 1: Install CMake via Snap (Easiest)

```bash
sudo apt update
sudo apt remove cmake  # Remove the old APT version
sudo snap install cmake --classic
```
*Note: You may need to restart your terminal session after installing so the `cmake` command points to the snap version.*

### Option 2: Install via Kitware's Official APT Repository

```bash
# 1. Remove the default cmake
sudo apt remove cmake

# 2. Add Kitware's GPG key and repository
wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | gpg --dearmor - | sudo tee /usr/share/keyrings/kitware-archive-keyring.gpg >/dev/null
echo 'deb [signed-by=/usr/share/keyrings/kitware-archive-keyring.gpg] https://apt.kitware.com/ubuntu/ jammy main' | sudo tee /etc/apt/sources.list.d/kitware.list >/dev/null

# 3. Update and install the latest cmake
sudo apt update
sudo apt install cmake
```

After using either option, verify your CMake version is 3.25 or higher:
```bash
cmake --version
```

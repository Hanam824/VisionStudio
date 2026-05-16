#!/usr/bin/env bash
# ═══════════════════════════════════════════════════════════════════════════════
# Vision Studio — macOS / Linux Build Script
# ═══════════════════════════════════════════════════════════════════════════════

set -e

# Change to the directory where the script is located
cd "$(dirname "$0")"

echo "=== Vision Studio Build ==="
echo

# 1. Detect platform
if [[ "$OSTYPE" == "darwin"* ]]; then
    PRESET="macos-x64"
    # Ensure macOS Ventura 13 is the minimum supported version
    export MACOSX_DEPLOYMENT_TARGET="13.0"
else
    PRESET="linux-x64"
fi

# Determine the preset prefix (e.g., macos or linux)
PRESET_PREFIX="${PRESET%%-*}"

echo -e "\033[36m>>> Vision Studio Build | Preset: $PRESET\033[0m"

# 2. macOS dependency installation
if [[ "$OSTYPE" == "darwin"* ]]; then
    echo -e "\033[33m>>> Checking macOS dependencies...\033[0m"
    
    # Check if Homebrew is installed
    if ! command -v brew &> /dev/null; then
        echo -e "\033[33m>>> Homebrew not found. Installing Homebrew...\033[0m"
        /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
        
        # Load brew into current shell session
        if [[ -x /opt/homebrew/bin/brew ]]; then
            eval "$(/opt/homebrew/bin/brew shellenv)"
        elif [[ -x /usr/local/bin/brew ]]; then
            eval "$(/usr/local/bin/brew shellenv)"
        fi
    fi
    
    # Install vcpkg system requirements for macOS
    echo -e "\033[33m>>> Installing essential macOS build tools...\033[0m"
    for pkg in autoconf autoconf-archive automake libtool; do
        if ! brew list "$pkg" &> /dev/null; then
            brew install "$pkg"
        fi
    done
fi

# 3. Check and initialize vcpkg submodule if empty
if [ ! -f "./third-party/vcpkg/bootstrap-vcpkg.sh" ]; then
    echo -e "\033[33m>>> vcpkg submodule is empty! Initializing git submodules...\033[0m"
    git submodule update --init --recursive
fi

# 3. Bootstrap vcpkg if needed
if [ ! -f "./third-party/vcpkg/vcpkg" ]; then
    echo -e "\033[33m>>> Bootstrapping vcpkg...\033[0m"
    ./third-party/vcpkg/bootstrap-vcpkg.sh -disableMetrics
fi

# 4. Configure
echo -e "\033[33m>>> Configuring...\033[0m"
if [[ "$OSTYPE" == "darwin"* ]]; then
    # We pass CMAKE_OSX_DEPLOYMENT_TARGET to ensure it's explicitly set for CMake
    cmake --preset "$PRESET" -DCMAKE_OSX_DEPLOYMENT_TARGET=13.0
else
    cmake --preset "$PRESET"
fi

# 5. Build Debug
echo -e "\033[33m>>> Building Debug...\033[0m"
cmake --build --preset "${PRESET_PREFIX}-debug" --parallel

# 6. Build Release
echo -e "\033[32m>>> Building Release...\033[0m"
cmake --build --preset "${PRESET_PREFIX}-release" --parallel

echo -e "\033[37m>>> Done! Check build/$PRESET directories\033[0m"

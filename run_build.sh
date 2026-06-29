#!/usr/bin/env bash
# ═══════════════════════════════════════════════════════════════════════════════
# Vision Studio — macOS / Linux Build Script
# ═══════════════════════════════════════════════════════════════════════════════

set -e

# Change to the directory where the script is located
cd "$(dirname "$0")"

echo "=== Vision Studio Build ==="
echo

# Set up local vcpkg binary cache
mkdir -p "$PWD/cache"
export VCPKG_BINARY_SOURCES="clear;files,$PWD/cache,readwrite"

# 1. Detect platform
if [[ "$OSTYPE" == "darwin"* ]]; then
    if [[ "$(uname -m)" == "arm64" ]]; then
        PRESET="macos-arm64"
    else
        PRESET="macos-x64"
    fi
else
    if command -v dpkg >/dev/null 2>&1; then
        PRESET="linux-deb-x64"
    elif command -v rpm >/dev/null 2>&1; then
        PRESET="linux-rpm-x64"
    else
        PRESET="linux-deb-x64"
    fi
fi

echo -e "\033[36m>>> Vision Studio Build | Preset: $PRESET\033[0m"

# 2. Dependency installation / checks
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
    for pkg in autoconf autoconf-archive automake libtool libomp; do
        if ! brew list "$pkg" &> /dev/null; then
            brew install "$pkg"
        fi
    done
else
    echo -e "\033[33m>>> Checking Linux dependencies...\033[0m"
    MISSING_PKGS=""
    if ! command -v cmake &> /dev/null; then MISSING_PKGS="$MISSING_PKGS cmake"; fi
    if ! command -v ninja &> /dev/null; then MISSING_PKGS="$MISSING_PKGS ninja-build"; fi
    if ! command -v git &> /dev/null; then MISSING_PKGS="$MISSING_PKGS git"; fi
    if ! command -v curl &> /dev/null; then MISSING_PKGS="$MISSING_PKGS curl"; fi
    if ! command -v zip &> /dev/null; then MISSING_PKGS="$MISSING_PKGS zip"; fi
    if ! command -v unzip &> /dev/null; then MISSING_PKGS="$MISSING_PKGS unzip"; fi
    if ! command -v tar &> /dev/null; then MISSING_PKGS="$MISSING_PKGS tar"; fi
    if ! command -v pkg-config &> /dev/null; then MISSING_PKGS="$MISSING_PKGS pkg-config"; fi
    
    if [ -n "$MISSING_PKGS" ]; then
        echo -e "\033[31m>>> Missing required tools:$MISSING_PKGS\033[0m"
        echo -e "\033[33m>>> Please install them using your package manager.\033[0m"
        if command -v apt-get &> /dev/null; then
            echo -e "    Example: sudo apt-get install build-essential$MISSING_PKGS"
        elif command -v dnf &> /dev/null; then
            echo -e "    Example: sudo dnf install gcc-c++$MISSING_PKGS"
        fi
        exit 1
    fi
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
cmake --preset "$PRESET"

# 5. Build Debug
# echo -e "\033[33m>>> Building Debug...\033[0m"
# cmake --build --preset "${PRESET}-debug" --parallel

# 6. Build Release
echo -e "\033[32m>>> Building Release...\033[0m"
cmake --build --preset "${PRESET}" --parallel

echo -e "\033[37m>>> Done! Check build/$PRESET directories\033[0m"

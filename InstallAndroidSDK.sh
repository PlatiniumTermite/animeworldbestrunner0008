#!/bin/bash

# Install Android SDK Command Line Tools for Anime World Runner
# This script downloads and installs the required Android SDK components

echo "📱 Installing Android SDK Command Line Tools"
echo "==========================================="

# Set Android SDK path
export ANDROID_HOME="$HOME/Library/Android/sdk"
export PATH="$ANDROID_HOME/cmdline-tools/latest/bin:$ANDROID_HOME/platform-tools:$PATH"

# Create SDK directory if it doesn't exist
mkdir -p "$ANDROID_HOME"

# Download Command Line Tools if not present
CMDLINE_TOOLS_DIR="$ANDROID_HOME/cmdline-tools"
if [ ! -d "$CMDLINE_TOOLS_DIR/latest" ]; then
    echo "⬇️  Downloading Android SDK Command Line Tools..."
    
    # Create temporary directory
    TEMP_DIR=$(mktemp -d)
    cd "$TEMP_DIR"
    
    # Download latest command line tools for macOS
    curl -o commandlinetools-mac.zip https://dl.google.com/android/repository/commandlinetools-mac-10406996_latest.zip
    
    if [ $? -eq 0 ]; then
        echo "📦 Extracting Command Line Tools..."
        unzip -q commandlinetools-mac.zip
        
        # Create proper directory structure
        mkdir -p "$CMDLINE_TOOLS_DIR"
        mv cmdline-tools "$CMDLINE_TOOLS_DIR/latest"
        
        echo "✅ Command Line Tools installed to: $CMDLINE_TOOLS_DIR/latest"
    else
        echo "❌ Failed to download Command Line Tools"
        echo "Please download manually from: https://developer.android.com/studio#command-tools"
        exit 1
    fi
    
    # Cleanup
    cd - > /dev/null
    rm -rf "$TEMP_DIR"
else
    echo "✅ Command Line Tools already installed"
fi

# Update PATH for current session
export PATH="$CMDLINE_TOOLS_DIR/latest/bin:$PATH"

# Accept licenses
echo "📝 Accepting Android SDK licenses..."
yes | sdkmanager --licenses > /dev/null 2>&1

# Install required SDK components
echo "📦 Installing required SDK components..."

# Install platform tools (adb, fastboot)
sdkmanager "platform-tools"

# Install build tools
sdkmanager "build-tools;33.0.0"

# Install Android 13 platform (API 33) - Samsung A56 target
sdkmanager "platforms;android-33"

# Install system image for emulator
sdkmanager "system-images;android-33;google_apis_playstore;arm64-v8a"

# Install emulator
sdkmanager "emulator"

# Install NDK for Unreal Engine
sdkmanager "ndk;25.1.8937393"

# Install CMake
sdkmanager "cmake;3.22.1"

echo "✅ Android SDK installation complete!"
echo ""
echo "📋 Installed components:"
echo "   - Platform Tools (adb, fastboot)"
echo "   - Build Tools 33.0.0"
echo "   - Android 13 Platform (API 33)"
echo "   - ARM64 System Image"
echo "   - Android Emulator"
echo "   - NDK 25.1.8937393"
echo "   - CMake 3.22.1"
echo ""

# Update shell profile
SHELL_PROFILE=""
if [ -f "$HOME/.zshrc" ]; then
    SHELL_PROFILE="$HOME/.zshrc"
elif [ -f "$HOME/.bash_profile" ]; then
    SHELL_PROFILE="$HOME/.bash_profile"
fi

if [ -n "$SHELL_PROFILE" ]; then
    echo "🔧 Updating shell profile: $SHELL_PROFILE"
    
    # Remove existing Android SDK entries
    grep -v "ANDROID_HOME\|android.*sdk" "$SHELL_PROFILE" > "${SHELL_PROFILE}.tmp" || true
    mv "${SHELL_PROFILE}.tmp" "$SHELL_PROFILE"
    
    # Add new Android SDK configuration
    cat >> "$SHELL_PROFILE" << EOF

# Android SDK for Anime World Runner
export ANDROID_HOME="\$HOME/Library/Android/sdk"
export PATH="\$ANDROID_HOME/cmdline-tools/latest/bin:\$ANDROID_HOME/platform-tools:\$ANDROID_HOME/emulator:\$PATH"
EOF
    
    echo "✅ Shell profile updated. Run 'source $SHELL_PROFILE' or restart terminal"
fi

echo ""
echo "🎯 Next steps:"
echo "1. Restart your terminal or run: source $SHELL_PROFILE"
echo "2. Run: ./CreateAndroidEmulator.sh"
echo "3. Run: ./QuickAndroidDeploy.sh"
echo ""
echo "🎌 Ready to deploy Anime World Runner to Android!"

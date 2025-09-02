#!/bin/bash

# Quick Android Deploy for Anime World Runner
# Simplified deployment script with better error handling

echo "🎌 Anime World Runner - Quick Android Deploy"
echo "==========================================="

# Set up Android SDK path
export ANDROID_HOME="$HOME/Library/Android/sdk"
export PATH="$ANDROID_HOME/platform-tools:$PATH"

# Project settings
PROJECT_DIR="/Users/divyanshurathore/Downloads/CascadeProjects/AnimeWorldRunner"
PROJECT_FILE="$PROJECT_DIR/AnimeWorldRunner.uproject"

echo "📱 Checking Android setup..."

# Check ADB
if ! command -v adb &> /dev/null; then
    echo "❌ ADB not found. Installing Android SDK Platform Tools..."
    echo "Please install Android Studio or download SDK Platform Tools"
    echo "Download from: https://developer.android.com/studio/releases/platform-tools"
    exit 1
fi

echo "✅ ADB found: $(which adb)"

# Check for devices
echo "🔍 Checking for connected devices..."
adb devices -l

DEVICE_COUNT=$(adb devices | grep -v "List of devices" | grep -E "device|emulator" | wc -l | tr -d ' ')

if [ "$DEVICE_COUNT" -eq 0 ]; then
    echo ""
    echo "❌ No Android devices found!"
    echo ""
    echo "📋 Setup Instructions:"
    echo "1. Connect your Samsung A56 (or any Android device) via USB"
    echo "2. Enable Developer Options:"
    echo "   - Go to Settings > About Phone"
    echo "   - Tap 'Build Number' 7 times"
    echo "3. Enable USB Debugging:"
    echo "   - Go to Settings > Developer Options"
    echo "   - Turn on 'USB Debugging'"
    echo "4. Allow USB debugging when prompted on device"
    echo "5. Run this script again"
    echo ""
    exit 1
fi

echo "✅ Found $DEVICE_COUNT Android device(s)"

# Check Unreal Engine
UE_PATHS=(
    "/Applications/Epic Games/UE_5.3/Engine/Build/BatchFiles/RunUAT.sh"
    "/Applications/Epic Games/UE_5.2/Engine/Build/BatchFiles/RunUAT.sh"
    "/Applications/Epic Games/UE_5.1/Engine/Build/BatchFiles/RunUAT.sh"
    "/Applications/UE_5.3/Engine/Build/BatchFiles/RunUAT.sh"
)

UE_BUILD_TOOL=""
for path in "${UE_PATHS[@]}"; do
    if [ -f "$path" ]; then
        UE_BUILD_TOOL="$path"
        break
    fi
done

if [ -z "$UE_BUILD_TOOL" ]; then
    echo "❌ Unreal Engine not found!"
    echo "Please install Unreal Engine 5.1+ from Epic Games Launcher"
    echo "Expected locations:"
    for path in "${UE_PATHS[@]}"; do
        echo "  - $path"
    done
    exit 1
fi

echo "✅ Found Unreal Engine: $UE_BUILD_TOOL"

# Check Java
if [ -z "$JAVA_HOME" ]; then
    # Try to find Java
    JAVA_PATHS=(
        "/Applications/Android Studio.app/Contents/jbr/Contents/Home"
        "/Library/Java/JavaVirtualMachines/*/Contents/Home"
        "/usr/libexec/java_home"
    )
    
    for path in "${JAVA_PATHS[@]}"; do
        if [ -d "$path" ]; then
            export JAVA_HOME="$path"
            break
        fi
    done
fi

echo "☕ Java Home: ${JAVA_HOME:-"System Default"}"

# Start build process
echo ""
echo "🏗️  Starting Android build process..."
echo "This may take 10-30 minutes depending on your Mac's performance"
echo ""

# Clean build
echo "🧹 Cleaning previous builds..."
rm -rf "$PROJECT_DIR/Saved/StagedBuilds"
rm -rf "$PROJECT_DIR/Binaries/Android"

# Create minimal build command for faster iteration
echo "📦 Building APK (Development build for faster compilation)..."

"$UE_BUILD_TOOL" BuildCookRun \
    -project="$PROJECT_FILE" \
    -platform=Android \
    -clientconfig=Development \
    -cook \
    -build \
    -stage \
    -package \
    -pak \
    -archive \
    -archivedirectory="$PROJECT_DIR/Builds" \
    -targetplatform=Android \
    -cookflavor=ASTC \
    -utf8output

BUILD_RESULT=$?

if [ $BUILD_RESULT -eq 0 ]; then
    echo ""
    echo "✅ Build completed successfully!"
    
    # Find APK
    APK_PATH=$(find "$PROJECT_DIR/Builds" -name "*.apk" -type f 2>/dev/null | head -1)
    
    if [ -n "$APK_PATH" ]; then
        echo "📦 APK: $APK_PATH"
        APK_SIZE=$(ls -lh "$APK_PATH" | awk '{print $5}')
        echo "📏 Size: $APK_SIZE"
        
        echo ""
        echo "📲 Installing on Android device..."
        
        # Uninstall previous version (ignore errors)
        adb uninstall com.animeworldrunner.game 2>/dev/null || true
        
        # Install new version
        adb install "$APK_PATH"
        
        if [ $? -eq 0 ]; then
            echo ""
            echo "🎉 SUCCESS! Anime World Runner installed on Android!"
            echo ""
            echo "🎮 Launch options:"
            echo "1. Tap the app icon on your device"
            echo "2. Or run: adb shell am start -n com.animeworldrunner.game/com.epicgames.unreal.GameActivity"
            echo ""
            echo "📊 Performance Tips for Samsung A56:"
            echo "- Game targets 60 FPS with dynamic resolution"
            echo "- Touch controls are enabled for mobile"
            echo "- Graphics automatically optimize for your device"
            echo ""
            
            # Optional auto-launch
            read -p "🚀 Launch game now? (y/n): " -n 1 -r
            echo
            if [[ $REPLY =~ ^[Yy]$ ]]; then
                echo "🎮 Launching Anime World Runner..."
                adb shell am start -n com.animeworldrunner.game/com.epicgames.unreal.GameActivity
                echo "✨ Game should be starting on your device!"
            fi
        else
            echo "❌ Installation failed"
            echo "Try manually installing: adb install \"$APK_PATH\""
        fi
    else
        echo "❌ APK not found after build"
        echo "Check build logs above for errors"
    fi
else
    echo ""
    echo "❌ Build failed with exit code: $BUILD_RESULT"
    echo ""
    echo "🔧 Common solutions:"
    echo "1. Make sure Android SDK is properly installed"
    echo "2. Check that NDK is installed in Android Studio"
    echo "3. Verify Java JDK is installed"
    echo "4. Try building from Unreal Editor first"
    echo ""
fi

#!/bin/bash

# Simple Android Deploy - No Emulator Required
# Direct deployment to connected Android device

echo "🎌 Anime World Runner - Simple Android Deploy"
echo "============================================="

# Set Android SDK path
export ANDROID_HOME="$HOME/Library/Android/sdk"
export PATH="$ANDROID_HOME/platform-tools:$PATH"

# Project settings
PROJECT_DIR="/Users/divyanshurathore/Downloads/CascadeProjects/AnimeWorldRunner"
PROJECT_FILE="$PROJECT_DIR/AnimeWorldRunner.uproject"

# Check ADB
if ! command -v adb &> /dev/null; then
    echo "❌ ADB not found at: $ANDROID_HOME/platform-tools/adb"
    echo ""
    echo "📋 Quick Fix:"
    echo "1. Install Android Studio from: https://developer.android.com/studio"
    echo "2. Or download SDK Platform Tools: https://developer.android.com/studio/releases/platform-tools"
    echo "3. Extract to: $ANDROID_HOME/platform-tools/"
    exit 1
fi

echo "✅ ADB found: $(which adb)"

# Check for connected device
echo "🔍 Checking for connected devices..."
adb devices -l

DEVICE_COUNT=$(adb devices | grep -v "List of devices" | grep "device" | wc -l | tr -d ' ')

if [ "$DEVICE_COUNT" -eq 0 ]; then
    echo ""
    echo "❌ No Android devices connected!"
    echo ""
    echo "📱 Connect your Android device (Samsung, Google Pixel, OnePlus, etc.):"
    echo "1. Connect via USB cable"
    echo "2. Enable Developer Options: Settings → About Phone → Tap 'Build Number' 7 times"
    echo "3. Enable USB Debugging: Settings → Developer Options → USB Debugging (ON)"
    echo "4. Allow debugging when prompted on device"
    echo ""
    echo "Then run this script again!"
    exit 1
fi

echo "✅ Found $DEVICE_COUNT Android device(s)"

# Find Unreal Engine
UE_PATHS=(
    "/Applications/Epic Games/UE_5.3/Engine/Build/BatchFiles/RunUAT.sh"
    "/Applications/Epic Games/UE_5.2/Engine/Build/BatchFiles/RunUAT.sh"
    "/Applications/Epic Games/UE_5.1/Engine/Build/BatchFiles/RunUAT.sh"
    "/Applications/UE_5.3/Engine/Build/BatchFiles/RunUAT.sh"
    "/Users/Shared/Epic Games/UE_5.3/Engine/Build/BatchFiles/RunUAT.sh"
)

UE_BUILD_TOOL=""
for path in "${UE_PATHS[@]}"; do
    if [ -f "$path" ]; then
        UE_BUILD_TOOL="$path"
        echo "✅ Found Unreal Engine: $path"
        break
    fi
done

if [ -z "$UE_BUILD_TOOL" ]; then
    echo "❌ Unreal Engine not found!"
    echo ""
    echo "📋 Install Unreal Engine:"
    echo "1. Download Epic Games Launcher: https://www.epicgames.com/store/download"
    echo "2. Install Unreal Engine 5.3"
    echo "3. Make sure to install Android support"
    exit 1
fi

# Set Java if needed
if [ -z "$JAVA_HOME" ]; then
    if [ -d "/Applications/Android Studio.app/Contents/jbr/Contents/Home" ]; then
        export JAVA_HOME="/Applications/Android Studio.app/Contents/jbr/Contents/Home"
        echo "✅ Using Java from Android Studio"
    fi
fi

# Clean previous builds
echo "🧹 Cleaning previous builds..."
rm -rf "$PROJECT_DIR/Saved/StagedBuilds/Android*"
rm -rf "$PROJECT_DIR/Builds"

# Build for Android
echo ""
echo "🏗️  Building Anime World Runner for Android..."
echo "⏱️  This will take 15-30 minutes..."
echo ""

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
    -utf8output \
    -nocompile \
    -nocompileeditor

BUILD_RESULT=$?

if [ $BUILD_RESULT -eq 0 ]; then
    echo ""
    echo "🎉 Build completed successfully!"
    
    # Find the APK
    APK_PATH=$(find "$PROJECT_DIR/Builds" -name "*.apk" -type f 2>/dev/null | head -1)
    
    if [ -n "$APK_PATH" ]; then
        echo "📦 APK: $APK_PATH"
        
        # Get APK info
        APK_SIZE=$(ls -lh "$APK_PATH" | awk '{print $5}')
        echo "📏 Size: $APK_SIZE"
        
        # Install on device
        echo ""
        echo "📲 Installing on Android device..."
        
        # Uninstall old version
        adb uninstall com.animeworldrunner.game 2>/dev/null || true
        
        # Install new version
        adb install "$APK_PATH"
        
        if [ $? -eq 0 ]; then
            echo ""
            echo "🎉 SUCCESS! Anime World Runner installed!"
            echo ""
            echo "🎮 Your Genshin Impact-style anime runner is ready!"
            echo ""
            echo "📱 Features on your Android device:"
            echo "   ✅ 3D movement with dash, glide, climb abilities"
            echo "   ✅ Cel-shaded anime-style graphics"
            echo "   ✅ Touch controls optimized for Android devices"
            echo "   ✅ Character customization system"
            echo "   ✅ Procedural environments"
            echo "   ✅ Adaptive performance for modern Android devices"
            echo ""
            
            # Launch game
            read -p "🚀 Launch game now? (y/n): " -n 1 -r
            echo
            if [[ $REPLY =~ ^[Yy]$ ]]; then
                echo "🎮 Starting Anime World Runner..."
                adb shell am start -n com.animeworldrunner.game/com.epicgames.unreal.GameActivity
                echo "✨ Game launched on your device!"
            fi
        else
            echo "❌ Installation failed"
        fi
    else
        echo "❌ APK not found"
    fi
else
    echo ""
    echo "❌ Build failed"
    echo "Check the output above for errors"
fi

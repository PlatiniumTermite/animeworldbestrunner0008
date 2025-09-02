#!/bin/bash

# Anime World Runner - Android Build and Deploy Script
# This script builds the project for Android and deploys to connected device

set -e  # Exit on any error

echo "🎌 Anime World Runner - Android Build & Deploy"
echo "=============================================="

# Project paths
PROJECT_DIR="/Users/divyanshurathore/Downloads/CascadeProjects/AnimeWorldRunner"
PROJECT_FILE="$PROJECT_DIR/AnimeWorldRunner.uproject"
BUILD_DIR="$PROJECT_DIR/Saved/StagedBuilds/Android_ASTC"

# Check if Unreal Engine is installed
if ! command -v "/Applications/Epic Games/UE_5.3/Engine/Build/BatchFiles/RunUAT.sh" &> /dev/null; then
    echo "❌ Unreal Engine 5.3 not found at expected location"
    echo "Please install UE 5.3 or update the path in this script"
    exit 1
fi

# Check if Android SDK is configured
if [ -z "$ANDROID_HOME" ]; then
    echo "⚠️  ANDROID_HOME not set. Setting default path..."
    export ANDROID_HOME="$HOME/Library/Android/sdk"
fi

if [ -z "$JAVA_HOME" ]; then
    echo "⚠️  JAVA_HOME not set. Setting default path..."
    export JAVA_HOME="/Applications/Android Studio.app/Contents/jbr/Contents/Home"
fi

echo "📱 Android SDK: $ANDROID_HOME"
echo "☕ Java Home: $JAVA_HOME"

# Check for connected Android device
echo "🔍 Checking for connected Android devices..."
if ! command -v adb &> /dev/null; then
    echo "❌ ADB not found. Please install Android SDK Platform Tools"
    exit 1
fi

DEVICES=$(adb devices | grep -v "List of devices" | grep "device$" | wc -l)
if [ $DEVICES -eq 0 ]; then
    echo "❌ No Android devices connected"
    echo "Please connect your Samsung A56 or other Android device and enable USB debugging"
    exit 1
fi

echo "✅ Found $DEVICES connected Android device(s)"

# Clean previous builds
echo "🧹 Cleaning previous builds..."
rm -rf "$BUILD_DIR"
rm -rf "$PROJECT_DIR/Binaries"
rm -rf "$PROJECT_DIR/Intermediate"

# Generate project files
echo "🔧 Generating project files..."
"/Applications/Epic Games/UE_5.3/Engine/Binaries/DotNET/UnrealBuildTool/UnrealBuildTool" -projectfiles -project="$PROJECT_FILE" -game -rocket -progress

# Build the project for Android
echo "🏗️  Building for Android (ARM64)..."
"/Applications/Epic Games/UE_5.3/Engine/Build/BatchFiles/RunUAT.sh" BuildCookRun \
    -project="$PROJECT_FILE" \
    -platform=Android \
    -clientconfig=Shipping \
    -serverconfig=Shipping \
    -cook \
    -allmaps \
    -build \
    -stage \
    -package \
    -archive \
    -archivedirectory="$PROJECT_DIR/Builds" \
    -pak \
    -prereqs \
    -nodebuginfo \
    -targetplatform=Android \
    -cookflavor=ASTC \
    -compressed

if [ $? -eq 0 ]; then
    echo "✅ Build completed successfully!"
    
    # Find the generated APK
    APK_PATH=$(find "$PROJECT_DIR/Builds" -name "*.apk" -type f | head -1)
    
    if [ -n "$APK_PATH" ]; then
        echo "📦 APK found: $APK_PATH"
        
        # Install on connected device
        echo "📲 Installing on Android device..."
        adb install -r "$APK_PATH"
        
        if [ $? -eq 0 ]; then
            echo "🎉 Successfully installed Anime World Runner on Android!"
            echo "🚀 You can now launch the game on your device"
            
            # Optional: Launch the app
            read -p "Launch the app now? (y/n): " -n 1 -r
            echo
            if [[ $REPLY =~ ^[Yy]$ ]]; then
                adb shell am start -n com.animeworldrunner.game/com.epicgames.unreal.GameActivity
                echo "🎮 Game launched!"
            fi
        else
            echo "❌ Failed to install APK on device"
            exit 1
        fi
    else
        echo "❌ APK not found in build directory"
        exit 1
    fi
else
    echo "❌ Build failed"
    exit 1
fi

echo "✨ Deployment complete!"

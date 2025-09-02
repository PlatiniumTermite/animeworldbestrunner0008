#!/bin/bash

# Quick Start - Anime World Runner
# One-click solution to run the game

echo "🎌 Anime World Runner - Quick Start"
echo "=================================="

# Check if Android device is connected
export ANDROID_HOME="$HOME/Library/Android/sdk"
export PATH="$ANDROID_HOME/platform-tools:$PATH"

if command -v adb &> /dev/null; then
    DEVICE_COUNT=$(adb devices | grep -v "List of devices" | grep "device" | wc -l | tr -d ' ')
    
    if [ "$DEVICE_COUNT" -gt 0 ]; then
        echo "📱 Android device detected!"
        echo "🚀 Deploying to your device..."
        ./SimpleAndroidDeploy.sh
        exit 0
    fi
fi

echo "📱 No Android device connected."
echo ""
echo "🎯 Choose your option:"
echo "1. Connect Android device and deploy"
echo "2. Run with Android emulator"
echo "3. Exit"
echo ""

read -p "Enter choice (1-3): " choice

case $choice in
    1)
        echo ""
        echo "📋 Steps to connect your Android device:"
        echo "1. Connect via USB cable"
        echo "2. Enable Developer Options: Settings → About Phone → Tap 'Build Number' 7 times"
        echo "3. Enable USB Debugging: Settings → Developer Options → USB Debugging (ON)"
        echo "4. Allow debugging when prompted on device"
        echo ""
        echo "Then run this script again!"
        ;;
    2)
        echo "🚀 Starting with emulator..."
        ./RunWithEmulator.sh
        ;;
    3)
        echo "👋 Goodbye!"
        exit 0
        ;;
    *)
        echo "❌ Invalid choice"
        exit 1
        ;;
esac

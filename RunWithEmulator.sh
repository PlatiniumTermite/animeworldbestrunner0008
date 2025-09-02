#!/bin/bash

# Run Anime World Runner with Android Emulator
# Creates and launches emulator, then deploys the game

echo "🎌 Anime World Runner - Emulator Launch"
echo "======================================"

# Set Android SDK path
export ANDROID_HOME="$HOME/Library/Android/sdk"
export PATH="$ANDROID_HOME/cmdline-tools/latest/bin:$ANDROID_HOME/platform-tools:$ANDROID_HOME/emulator:$PATH"

# Check if emulator exists
AVD_NAME="AnimeRunner_Device"
if ! avdmanager list avd | grep -q "$AVD_NAME"; then
    echo "📱 Creating Android emulator..."
    
    # Accept licenses
    yes | sdkmanager --licenses > /dev/null 2>&1
    
    # Install system image if not present
    sdkmanager "system-images;android-33;google_apis_playstore;arm64-v8a" > /dev/null 2>&1
    
    # Create AVD
    echo "no" | avdmanager create avd -n "$AVD_NAME" -k "system-images;android-33;google_apis_playstore;arm64-v8a" -d "pixel_6"
    
    # Configure for gaming performance
    AVD_CONFIG="$HOME/.android/avd/${AVD_NAME}.avd/config.ini"
    if [ -f "$AVD_CONFIG" ]; then
        cat >> "$AVD_CONFIG" << EOF
hw.ramSize=6144
hw.heapSize=512
hw.gpu.enabled=yes
hw.gpu.mode=auto
hw.keyboard=yes
disk.dataPartition.size=8192M
EOF
    fi
    
    echo "✅ Emulator created: $AVD_NAME"
fi

# Start emulator in background
echo "🚀 Starting Android emulator..."
echo "This may take 2-3 minutes to boot..."

emulator -avd "$AVD_NAME" -memory 6144 -cores 4 -gpu swiftshader_indirect > /dev/null 2>&1 &
EMULATOR_PID=$!

# Wait for device to be ready
echo "⏳ Waiting for emulator to boot..."
timeout=300  # 5 minutes timeout
elapsed=0

while [ $elapsed -lt $timeout ]; do
    if adb devices | grep -q "emulator.*device"; then
        echo "✅ Emulator is ready!"
        break
    fi
    sleep 5
    elapsed=$((elapsed + 5))
    echo "   Still booting... ($elapsed/${timeout}s)"
done

if [ $elapsed -ge $timeout ]; then
    echo "❌ Emulator failed to start within 5 minutes"
    kill $EMULATOR_PID 2>/dev/null
    exit 1
fi

# Now deploy the game
echo ""
echo "🎮 Deploying Anime World Runner to emulator..."
./SimpleAndroidDeploy.sh

echo ""
echo "🎉 Game should now be running in the emulator!"
echo "🎌 Enjoy your Genshin Impact-style anime runner!"

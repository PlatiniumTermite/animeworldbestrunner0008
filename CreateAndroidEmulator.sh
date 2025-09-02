#!/bin/bash

# Create Android Emulator for Anime World Runner Testing
# This script creates a Samsung A56-like emulator for testing

echo "📱 Creating Android Emulator for Anime World Runner"
echo "================================================="

# Set Android SDK path
export ANDROID_HOME="$HOME/Library/Android/sdk"
export PATH="$ANDROID_HOME/emulator:$ANDROID_HOME/tools:$ANDROID_HOME/platform-tools:$PATH"

# Check if Android SDK is installed
if [ ! -d "$ANDROID_HOME" ]; then
    echo "❌ Android SDK not found at $ANDROID_HOME"
    echo "Please install Android Studio or SDK Command Line Tools"
    exit 1
fi

echo "✅ Android SDK found: $ANDROID_HOME"

# Check for required tools
if ! command -v avdmanager &> /dev/null; then
    echo "❌ avdmanager not found. Please install Android SDK Command Line Tools"
    exit 1
fi

if ! command -v sdkmanager &> /dev/null; then
    echo "❌ sdkmanager not found. Please install Android SDK Command Line Tools"
    exit 1
fi

# Install required SDK components
echo "📦 Installing required SDK components..."

# Accept licenses
yes | sdkmanager --licenses > /dev/null 2>&1

# Install system image for API 33 (Android 13) - Samsung A56 equivalent
echo "⬇️  Installing Android 13 system image..."
sdkmanager "system-images;android-33;google_apis_playstore;arm64-v8a"

# Install platform tools
echo "⬇️  Installing platform tools..."
sdkmanager "platform-tools" "platforms;android-33" "build-tools;33.0.0"

# Create AVD (Android Virtual Device)
AVD_NAME="Samsung_A56_Emulator"
echo "🔧 Creating emulator: $AVD_NAME"

# Delete existing AVD if it exists
avdmanager delete avd -n "$AVD_NAME" 2>/dev/null || true

# Create new AVD with Samsung A56-like specs
avdmanager create avd \
    -n "$AVD_NAME" \
    -k "system-images;android-33;google_apis_playstore;arm64-v8a" \
    -d "pixel_6" \
    --force

# Configure AVD settings
AVD_CONFIG="$HOME/.android/avd/${AVD_NAME}.avd/config.ini"

if [ -f "$AVD_CONFIG" ]; then
    echo "⚙️  Configuring emulator settings..."
    
    # Samsung A56 specifications
    cat >> "$AVD_CONFIG" << EOF

# Samsung A56-like configuration
hw.ramSize=6144
hw.heapSize=512
vm.heapSize=256
hw.lcd.density=420
hw.lcd.width=1080
hw.lcd.height=2340
hw.gpu.enabled=yes
hw.gpu.mode=auto
hw.keyboard=yes
hw.sensors.orientation=yes
hw.sensors.proximity=yes
hw.dPad=no
hw.gsmModem=yes
hw.gps=yes
hw.battery=yes
hw.accelerometer=yes
hw.gyroscope=yes
hw.audioInput=yes
hw.audioOutput=yes
hw.sdCard=yes
sdcard.size=2048M
hw.camera.back=webcam0
hw.camera.front=webcam0
hw.cpu.ncore=4
disk.dataPartition.size=8192M
EOF

    echo "✅ Emulator configured with Samsung A56-like specs:"
    echo "   - RAM: 6GB"
    echo "   - Storage: 8GB"
    echo "   - Resolution: 1080x2340 (FHD+)"
    echo "   - Android 13 (API 33)"
    echo "   - ARM64 architecture"
fi

# Create launch script
LAUNCH_SCRIPT="$HOME/Desktop/Launch_AnimeRunner_Emulator.sh"
cat > "$LAUNCH_SCRIPT" << 'EOF'
#!/bin/bash
export ANDROID_HOME="$HOME/Library/Android/sdk"
export PATH="$ANDROID_HOME/emulator:$ANDROID_HOME/platform-tools:$PATH"

echo "🚀 Starting Samsung A56 Emulator for Anime World Runner..."
echo "This may take 2-3 minutes to boot up..."

emulator -avd Samsung_A56_Emulator \
    -memory 6144 \
    -cores 4 \
    -gpu swiftshader_indirect \
    -skin 1080x2340 \
    -camera-back webcam0 \
    -camera-front webcam0 \
    -qemu -m 6144 &

echo "✅ Emulator starting in background..."
echo "📱 Wait for Android to boot, then run: ./QuickAndroidDeploy.sh"
EOF

chmod +x "$LAUNCH_SCRIPT"

echo ""
echo "🎉 Android emulator created successfully!"
echo ""
echo "🚀 To start the emulator:"
echo "   1. Double-click: $LAUNCH_SCRIPT"
echo "   2. Or run: emulator -avd $AVD_NAME"
echo ""
echo "📱 Once emulator is running:"
echo "   1. Wait for Android to fully boot (2-3 minutes)"
echo "   2. Run: ./QuickAndroidDeploy.sh"
echo "   3. Your Anime World Runner will install and launch!"
echo ""
echo "⚡ Performance Tips:"
echo "   - Close other apps to free up RAM"
echo "   - Enable hardware acceleration in BIOS"
echo "   - Use SSD for better performance"
echo ""

# Optional: Start emulator immediately
read -p "🚀 Start emulator now? (y/n): " -n 1 -r
echo
if [[ $REPLY =~ ^[Yy]$ ]]; then
    echo "🎮 Starting emulator..."
    emulator -avd "$AVD_NAME" -memory 6144 -cores 4 -gpu swiftshader_indirect &
    echo "✅ Emulator starting in background..."
    echo "📱 Wait for Android to boot, then run: ./QuickAndroidDeploy.sh"
fi

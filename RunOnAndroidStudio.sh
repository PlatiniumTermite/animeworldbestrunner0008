#!/bin/bash

# Run Anime World Runner on Android Studio Emulator
# Uses existing Android Studio emulator if available

echo "🎌 Anime World Runner - Android Studio Emulator"
echo "=============================================="

# Set Android SDK paths (Android Studio locations)
ANDROID_STUDIO_SDK="$HOME/Library/Android/sdk"
ANDROID_HOME_PATHS=(
    "$ANDROID_STUDIO_SDK"
    "$HOME/Android/Sdk"
    "/Applications/Android Studio.app/Contents/sdk"
)

# Find Android SDK
ANDROID_HOME=""
for path in "${ANDROID_HOME_PATHS[@]}"; do
    if [ -d "$path" ]; then
        ANDROID_HOME="$path"
        echo "✅ Found Android SDK: $path"
        break
    fi
done

if [ -z "$ANDROID_HOME" ]; then
    echo "❌ Android Studio SDK not found!"
    echo ""
    echo "📋 Install Android Studio:"
    echo "1. Download from: https://developer.android.com/studio"
    echo "2. Install with SDK components"
    echo "3. Create an emulator in AVD Manager"
    exit 1
fi

export ANDROID_HOME
export PATH="$ANDROID_HOME/cmdline-tools/latest/bin:$ANDROID_HOME/platform-tools:$ANDROID_HOME/emulator:$PATH"

# Check for existing emulators
echo "🔍 Checking for Android Studio emulators..."

if ! command -v emulator &> /dev/null; then
    echo "❌ Emulator command not found"
    echo "Make sure Android Studio is properly installed"
    exit 1
fi

# List available AVDs
AVDS=$(emulator -list-avds 2>/dev/null)

if [ -z "$AVDS" ]; then
    echo "❌ No Android emulators found!"
    echo ""
    echo "📱 Create an emulator in Android Studio:"
    echo "1. Open Android Studio"
    echo "2. Go to Tools → AVD Manager"
    echo "3. Click 'Create Virtual Device'"
    echo "4. Choose Pixel 6 or similar (API 33+)"
    echo "5. Download system image if needed"
    echo "6. Create and start the emulator"
    echo ""
    echo "Then run this script again!"
    exit 1
fi

echo "📱 Available emulators:"
echo "$AVDS" | nl -w2 -s'. '

# Auto-select first emulator or let user choose
AVD_ARRAY=($AVDS)
SELECTED_AVD="${AVD_ARRAY[0]}"

if [ ${#AVD_ARRAY[@]} -gt 1 ]; then
    echo ""
    read -p "Enter emulator number (1-${#AVD_ARRAY[@]}) or press Enter for first: " choice
    if [[ "$choice" =~ ^[0-9]+$ ]] && [ "$choice" -ge 1 ] && [ "$choice" -le ${#AVD_ARRAY[@]} ]; then
        SELECTED_AVD="${AVD_ARRAY[$((choice-1))]}"
    fi
fi

echo "🚀 Starting emulator: $SELECTED_AVD"

# Check if emulator is already running
if adb devices | grep -q "emulator.*device"; then
    echo "✅ Emulator already running!"
else
    echo "⏳ Starting emulator (this may take 2-3 minutes)..."
    emulator -avd "$SELECTED_AVD" -memory 6144 -cores 4 > /dev/null 2>&1 &
    
    # Wait for emulator to boot
    timeout=300
    elapsed=0
    
    while [ $elapsed -lt $timeout ]; do
        if adb devices | grep -q "emulator.*device"; then
            echo "✅ Emulator is ready!"
            break
        fi
        sleep 5
        elapsed=$((elapsed + 5))
        echo "   Booting... ($elapsed/${timeout}s)"
    done
    
    if [ $elapsed -ge $timeout ]; then
        echo "❌ Emulator failed to start"
        exit 1
    fi
fi

# Deploy the game
echo ""
echo "🎮 Deploying Anime World Runner to Android Studio emulator..."

# Find Unreal Engine
UE_PATHS=(
    "/Applications/Epic Games/UE_5.3/Engine/Build/BatchFiles/RunUAT.sh"
    "/Applications/Epic Games/UE_5.2/Engine/Build/BatchFiles/RunUAT.sh"
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
    echo "Install Unreal Engine 5.3 from Epic Games Launcher"
    exit 1
fi

PROJECT_DIR="/Users/divyanshurathore/Downloads/CascadeProjects/AnimeWorldRunner"
PROJECT_FILE="$PROJECT_DIR/AnimeWorldRunner.uproject"

# Clean and build
echo "🧹 Cleaning previous builds..."
rm -rf "$PROJECT_DIR/Saved/StagedBuilds/Android*"

echo "🏗️  Building for Android emulator..."
echo "⏱️  This will take 15-25 minutes..."

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

if [ $? -eq 0 ]; then
    # Find and install APK
    APK_PATH=$(find "$PROJECT_DIR/Builds" -name "*.apk" -type f 2>/dev/null | head -1)
    
    if [ -n "$APK_PATH" ]; then
        echo "📦 Installing APK on emulator..."
        adb uninstall com.animeworldrunner.game 2>/dev/null || true
        adb install "$APK_PATH"
        
        if [ $? -eq 0 ]; then
            echo ""
            echo "🎉 SUCCESS! Game installed on Android Studio emulator!"
            echo ""
            echo "🎮 Your Genshin Impact-style anime runner is ready!"
            echo "📱 Features:"
            echo "   ✅ 3D movement with anime abilities"
            echo "   ✅ Touch controls on emulator"
            echo "   ✅ Character customization"
            echo "   ✅ Procedural environments"
            echo ""
            
            read -p "🚀 Launch game now? (y/n): " -n 1 -r
            echo
            if [[ $REPLY =~ ^[Yy]$ ]]; then
                adb shell am start -n com.animeworldrunner.game/com.epicgames.unreal.GameActivity
                echo "✨ Game launched in Android Studio emulator!"
            fi
        else
            echo "❌ Installation failed"
        fi
    else
        echo "❌ APK not found"
    fi
else
    echo "❌ Build failed"
fi

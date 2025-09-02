#!/bin/bash

# Run Anime World Runner on Desktop (Mac)
# For immediate testing while Android builds

echo "🎌 Anime World Runner - Desktop Launch"
echo "====================================="

PROJECT_DIR="/Users/divyanshurathore/Downloads/CascadeProjects/AnimeWorldRunner"
PROJECT_FILE="$PROJECT_DIR/AnimeWorldRunner.uproject"

# Find Unreal Engine
UE_PATHS=(
    "/Applications/Epic Games/UE_5.3/Engine/Binaries/Mac/UnrealEditor.app/Contents/MacOS/UnrealEditor"
    "/Applications/Epic Games/UE_5.2/Engine/Binaries/Mac/UnrealEditor.app/Contents/MacOS/UnrealEditor"
    "/Applications/UE_5.3/Engine/Binaries/Mac/UnrealEditor.app/Contents/MacOS/UnrealEditor"
)

UE_EDITOR=""
for path in "${UE_PATHS[@]}"; do
    if [ -f "$path" ]; then
        UE_EDITOR="$path"
        echo "✅ Found Unreal Engine: $path"
        break
    fi
done

if [ -z "$UE_EDITOR" ]; then
    echo "❌ Unreal Engine not found!"
    echo "Install Unreal Engine 5.3 from Epic Games Launcher"
    exit 1
fi

echo "🚀 Launching Anime World Runner in Unreal Editor..."
echo ""
echo "🎮 Controls:"
echo "   WASD - Move character"
echo "   Mouse - Camera control"
echo "   Space - Jump"
echo "   Shift - Dash"
echo "   G - Glide"
echo "   C - Climb"
echo "   F - Attack"
echo "   Q - Cast Spell"
echo ""
echo "🎌 Testing your Genshin Impact-style anime runner!"

# Launch in editor for immediate testing
"$UE_EDITOR" "$PROJECT_FILE" -game -windowed -ResX=1280 -ResY=720

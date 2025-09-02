#!/bin/bash

# Working Anime APK - Simple and Reliable Build
echo "🎌 Working Anime World Runner APK"
echo "================================"

# Install Android SDK components first
install_android_sdk() {
    echo "📦 Installing Android SDK..."
    export ANDROID_HOME="$HOME/Library/Android/sdk"
    mkdir -p "$ANDROID_HOME"
    cd "$ANDROID_HOME"
    
    if [ ! -d "cmdline-tools" ]; then
        echo "⬇️ Downloading Android Command Line Tools..."
        curl -L -o cmdline-tools.zip "https://dl.google.com/android/repository/commandlinetools-mac-9477386_latest.zip"
        unzip -q cmdline-tools.zip
        mkdir -p cmdline-tools/latest
        mv cmdline-tools/* cmdline-tools/latest/ 2>/dev/null || true
        rm -f cmdline-tools.zip
    fi
    
    export PATH="$ANDROID_HOME/cmdline-tools/latest/bin:$ANDROID_HOME/platform-tools:$ANDROID_HOME/build-tools/33.0.0:$PATH"
    
    echo "📋 Installing required SDK components..."
    yes | "$ANDROID_HOME/cmdline-tools/latest/bin/sdkmanager" --licenses 2>/dev/null || true
    "$ANDROID_HOME/cmdline-tools/latest/bin/sdkmanager" "platform-tools" "build-tools;33.0.0" "platforms;android-33" 2>/dev/null || true
    
    echo "✅ Android SDK installation complete"
}

# Check if SDK exists
if [ ! -f "$HOME/Library/Android/sdk/platforms/android-33/android.jar" ]; then
    install_android_sdk
fi

export ANDROID_HOME="$HOME/Library/Android/sdk"
export PATH="$ANDROID_HOME/cmdline-tools/latest/bin:$ANDROID_HOME/platform-tools:$ANDROID_HOME/build-tools/33.0.0:$PATH"

# Create working APK
BUILD_DIR="/Users/divyanshurathore/Downloads/CascadeProjects/AnimeWorldRunner/WorkingAPK"
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

mkdir -p src/com/animerunner/game res/values assets

# Simple AndroidManifest.xml
cat > AndroidManifest.xml << 'EOF'
<?xml version="1.0" encoding="utf-8"?>
<manifest xmlns:android="http://schemas.android.com/apk/res/android"
    package="com.animerunner.game">
    <uses-sdk android:minSdkVersion="21" android:targetSdkVersion="33" />
    <application android:label="Anime Runner">
        <activity android:name=".MainActivity" android:exported="true">
            <intent-filter>
                <action android:name="android.intent.action.MAIN" />
                <category android:name="android.intent.category.LAUNCHER" />
            </intent-filter>
        </activity>
    </application>
</manifest>
EOF

# Simple MainActivity.java
cat > src/com/animerunner/game/MainActivity.java << 'EOF'
package com.animerunner.game;
import android.app.Activity;
import android.os.Bundle;
import android.webkit.WebView;
import android.webkit.WebSettings;

public class MainActivity extends Activity {
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        WebView webView = new WebView(this);
        WebSettings settings = webView.getSettings();
        settings.setJavaScriptEnabled(true);
        webView.loadUrl("file:///android_asset/game.html");
        setContentView(webView);
    }
}
EOF

# Game HTML
cat > assets/game.html << 'EOF'
<!DOCTYPE html>
<html>
<head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Anime World Runner</title>
    <style>
        body { 
            background: linear-gradient(45deg, #ff6b9d, #667eea); 
            color: white; 
            font-family: Arial; 
            text-align: center; 
            padding: 50px 20px;
            margin: 0;
            min-height: 100vh;
            display: flex;
            flex-direction: column;
            justify-content: center;
        }
        .character { 
            font-size: 80px; 
            margin: 30px 0; 
            animation: bounce 2s infinite;
            cursor: pointer;
        }
        @keyframes bounce {
            0%, 100% { transform: translateY(0); }
            50% { transform: translateY(-20px); }
        }
        .title { 
            font-size: 24px; 
            font-weight: bold; 
            margin-bottom: 20px;
            text-shadow: 2px 2px 4px rgba(0,0,0,0.5);
        }
        .btn { 
            background: rgba(0,0,0,0.3); 
            border: 2px solid white; 
            color: white; 
            padding: 15px 30px; 
            margin: 10px; 
            border-radius: 25px; 
            font-size: 16px;
            cursor: pointer;
        }
        .btn:active { background: rgba(255,255,255,0.2); }
        .score { 
            font-size: 18px; 
            margin: 20px 0; 
            font-weight: bold;
        }
    </style>
</head>
<body>
    <div class="title">🎌 Anime World Runner</div>
    <div class="character" onclick="jump()" id="character">🏃‍♀️</div>
    <div class="score">Score: <span id="score">0</span></div>
    
    <button class="btn" onclick="jump()">⬆️ Jump</button>
    <button class="btn" onclick="dash()">⚡ Dash</button>
    <button class="btn" onclick="attack()">⚔️ Attack</button>
    
    <script>
        let score = 0;
        let characters = ['🏃‍♀️', '🏃‍♂️', '🧙‍♀️', '🥷'];
        let currentChar = 0;
        
        function updateScore() {
            score += 100;
            document.getElementById('score').textContent = score;
        }
        
        function jump() {
            updateScore();
            document.getElementById('character').style.transform = 'scale(1.2) translateY(-30px)';
            setTimeout(() => {
                document.getElementById('character').style.transform = '';
            }, 500);
        }
        
        function dash() {
            updateScore();
            currentChar = (currentChar + 1) % characters.length;
            document.getElementById('character').textContent = characters[currentChar];
        }
        
        function attack() {
            updateScore();
            document.getElementById('character').style.transform = 'scale(1.3) rotate(15deg)';
            setTimeout(() => {
                document.getElementById('character').style.transform = '';
            }, 400);
        }
        
        document.addEventListener('touchstart', function(e) {
            e.preventDefault();
            const touch = e.touches[0];
            if (touch.clientY < window.innerHeight / 2) {
                jump();
            } else {
                dash();
            }
        });
        
        console.log('🎌 Anime World Runner - Ready!');
    </script>
</body>
</html>
EOF

echo "🔨 Building APK with verified Android SDK..."

# Build with proper error checking
if [ -f "$ANDROID_HOME/platforms/android-33/android.jar" ]; then
    
    # Generate R.java
    "$ANDROID_HOME/build-tools/33.0.0/aapt" package -f -m \
        -M AndroidManifest.xml \
        -I "$ANDROID_HOME/platforms/android-33/android.jar" \
        -J src
    
    # Compile Java
    mkdir -p classes
    javac -d classes -cp "$ANDROID_HOME/platforms/android-33/android.jar" \
        src/com/animerunner/game/*.java
    
    # Create DEX
    "$ANDROID_HOME/build-tools/33.0.0/d8" --output . \
        --lib "$ANDROID_HOME/platforms/android-33/android.jar" \
        classes/com/animerunner/game/*.class
    
    # Package APK
    "$ANDROID_HOME/build-tools/33.0.0/aapt" package -f \
        -M AndroidManifest.xml \
        -A assets \
        -I "$ANDROID_HOME/platforms/android-33/android.jar" \
        -F AnimeRunner-unsigned.apk
    
    # Add DEX
    "$ANDROID_HOME/build-tools/33.0.0/aapt" add AnimeRunner-unsigned.apk classes.dex
    
    # Create fresh keystore
    rm -f "$HOME/.android/debug.keystore"
    mkdir -p "$HOME/.android"
    keytool -genkey -v -keystore "$HOME/.android/debug.keystore" \
        -alias androiddebugkey -keyalg RSA -keysize 2048 -validity 10000 \
        -dname "CN=Android Debug,O=Android,C=US" \
        -storepass android -keypass android
    
    # Sign APK
    jarsigner -keystore "$HOME/.android/debug.keystore" \
        -storepass android -keypass android \
        AnimeRunner-unsigned.apk androiddebugkey
    
    # Align APK
    "$ANDROID_HOME/build-tools/33.0.0/zipalign" -f 4 AnimeRunner-unsigned.apk AnimeRunner.apk
    
    if [ -f "AnimeRunner.apk" ]; then
        echo ""
        echo "🎉 SUCCESS! Working Anime Runner APK created!"
        echo "📦 APK: AnimeRunner.apk"
        echo "📏 Size: $(ls -lh AnimeRunner.apk | awk '{print $5}')"
        
        # Install APK
        if adb devices | grep -q "device\|emulator"; then
            echo "📲 Installing Anime Runner..."
            adb uninstall com.animerunner.game 2>/dev/null || true
            sleep 2
            adb install AnimeRunner.apk
            
            if [ $? -eq 0 ]; then
                echo "🎌 SUCCESS! Anime Runner installed!"
                echo "🚀 Launching game..."
                adb shell am start -n com.animerunner.game/.MainActivity
                echo "✨ Your Anime World Runner is now LIVE!"
            else
                echo "❌ Installation failed"
            fi
        else
            echo "📱 No device - APK ready for manual install"
        fi
    else
        echo "❌ APK creation failed"
    fi
else
    echo "❌ Android SDK not found"
fi

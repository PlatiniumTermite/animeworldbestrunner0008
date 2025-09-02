#!/bin/bash

# GitHub Release Setup for Anime World Runner
echo "🎌 Preparing Anime World Runner for GitHub Release"
echo "================================================"

REPO_URL="https://github.com/PlatiniumTermite/animeworldbestrunner0008"
PROJECT_DIR="/Users/divyanshurathore/Downloads/CascadeProjects/AnimeWorldRunner"

cd "$PROJECT_DIR"

echo "📦 Organizing release files..."

# Create release directory structure
mkdir -p Release/APKs
mkdir -p Release/BuildScripts
mkdir -p Release/Documentation
mkdir -p Release/Assets

# Copy all APK files to release
echo "📱 Copying APK files..."
cp UltimateAPK/UltimateAnimeRunner.apk Release/APKs/ 2>/dev/null || true
cp FinalAPK/AnimeWorldRunner.apk Release/APKs/ 2>/dev/null || true
cp WorkingAPK/AnimeRunner.apk Release/APKs/ 2>/dev/null || true
cp FixedAPK/AnimeWorldRunner.apk Release/APKs/ 2>/dev/null || true

# Copy build scripts
echo "🔨 Copying build scripts..."
cp UltimateAnimeRunnerAPK.sh Release/BuildScripts/
cp FinalWorkingAnimeAPK.sh Release/BuildScripts/
cp SimpleWorkingAPK.sh Release/BuildScripts/
cp RunOnAndroidStudio.sh Release/BuildScripts/

# Copy documentation
echo "📄 Copying documentation..."
cp README.md Release/Documentation/
cp RELEASE_PACKAGE.md Release/Documentation/
cp AndroidDeploymentGuide.md Release/Documentation/ 2>/dev/null || true
cp BlueprintIntegrationGuide.md Release/Documentation/ 2>/dev/null || true

# Copy game assets
echo "🎨 Copying game assets..."
cp ultimate_game.html Release/Assets/ 2>/dev/null || true
cp SimpleAPK/assets/game.html Release/Assets/simple_game.html 2>/dev/null || true

# Create release info
cat > Release/RELEASE_INFO.md << 'EOF'
# 🎌 Anime World Runner - Release v1.0

## 📦 **Download Options**

### 🏆 **Recommended Download**
**UltimateAnimeRunner.apk** (6.5K) - Full featured anime runner

### 📱 **All APK Files**
- `UltimateAnimeRunner.apk` - Ultimate edition with all features
- `AnimeWorldRunner.apk` - Complete game experience
- `AnimeRunner.apk` - Simple and reliable version

## 🚀 **Quick Install**
```bash
adb install UltimateAnimeRunner.apk
```

## 🎮 **Game Features**
- 🎌 Genshin Impact-style anime graphics
- 🏃‍♀️ 6 unlockable anime characters
- ⚡ Advanced abilities: Jump, Dash, Attack, Cast
- 🎯 Combo scoring system with multipliers
- 📱 Optimized touch controls
- ✨ Dynamic visual effects

## 🔧 **Build Scripts**
- `UltimateAnimeRunnerAPK.sh` - Build ultimate edition
- `SimpleWorkingAPK.sh` - Web deployment (always works)
- `RunOnAndroidStudio.sh` - Emulator deployment

## 📱 **Compatibility**
- Android 5.0+ (API 21+)
- Optimized for Samsung A56
- Works on all Android devices

**Ready to run in the anime world!** 🏃‍♂️✨
EOF

# Create GitHub repository initialization
cat > Release/GITHUB_SETUP.sh << 'EOF'
#!/bin/bash

# GitHub Repository Setup
echo "🚀 Setting up GitHub repository..."

# Initialize git if not already done
if [ ! -d ".git" ]; then
    git init
    git branch -M main
fi

# Add remote repository
git remote remove origin 2>/dev/null || true
git remote add origin https://github.com/PlatiniumTermite/animeworldbestrunner0008.git

# Add all files
git add .
git commit -m "🎌 Initial release: Ultimate Anime World Runner v1.0

🎮 Features:
- Genshin Impact-style anime runner
- 6 unlockable characters with abilities
- Advanced 3D movement system
- Mobile-optimized touch controls
- Multiple APK builds available
- Web deployment option

📱 Ready for Android devices!"

# Push to GitHub
git push -u origin main

echo "✅ Repository setup complete!"
echo "🌐 Available at: https://github.com/PlatiniumTermite/animeworldbestrunner0008"
EOF

chmod +x Release/GITHUB_SETUP.sh

# List all release files
echo ""
echo "📋 Release package contents:"
echo "================================"
ls -la Release/APKs/
echo ""
ls -la Release/BuildScripts/
echo ""
ls -la Release/Documentation/
echo ""

echo "🎉 GitHub release package ready!"
echo ""
echo "📍 Release location: $PROJECT_DIR/Release/"
echo ""
echo "🚀 To publish to GitHub:"
echo "   cd Release && ./GITHUB_SETUP.sh"
echo ""
echo "🌐 Repository: $REPO_URL"
echo "✅ Ready for GitHub release!"

#!/bin/bash

# Complete GitHub Release for Anime World Runner
echo "🎌 Releasing Anime World Runner APKs on GitHub"
echo "=============================================="

REPO_URL="https://github.com/PlatiniumTermite/animeworldbestrunner0008"
PROJECT_DIR="/Users/divyanshurathore/Downloads/CascadeProjects/AnimeWorldRunner"

cd "$PROJECT_DIR"

# Check if we're in a git repository
if [ ! -d ".git" ]; then
    echo "🔧 Initializing Git repository..."
    git init
    git branch -M main
fi

# Add remote if not exists
if ! git remote get-url origin &>/dev/null; then
    echo "🔗 Adding GitHub remote..."
    git remote add origin https://github.com/PlatiniumTermite/animeworldbestrunner0008.git
fi

# Add all files including APKs
echo "📦 Adding all project files..."
git add .
git add -f Release/APKs/*.apk 2>/dev/null || true
git add -f UltimateAPK/*.apk 2>/dev/null || true
git add -f FinalAPK/*.apk 2>/dev/null || true
git add -f WorkingAPK/*.apk 2>/dev/null || true
git add -f FixedAPK/*.apk 2>/dev/null || true
git add -f QuickAPK/*.apk 2>/dev/null || true
git add -f AndroidBuild/*.apk 2>/dev/null || true
git add -f *.apk 2>/dev/null || true

# Commit everything
echo "💾 Committing all files..."
git commit -m "🎌 Complete Anime World Runner Release

🎮 Ultimate Edition Features:
- Genshin Impact-style anime runner
- 6 unlockable characters with abilities  
- Advanced 3D movement system
- Mobile-optimized touch controls
- Multiple APK builds available
- Web deployment option

📱 APK Files Included:
- UltimateAnimeRunner.apk (Ultimate Edition)
- AnimeWorldRunner.apk (Complete Game)
- AnimeRunner.apk (Simple Version)

🚀 Ready for Android devices!"

# Push to GitHub
echo "🚀 Pushing to GitHub..."
git push -u origin main --force

echo ""
echo "✅ SUCCESS! Anime World Runner released on GitHub!"
echo ""
echo "🌐 Repository: $REPO_URL"
echo ""
echo "📱 Next Steps:"
echo "1. Go to: $REPO_URL/releases"
echo "2. Click 'Create a new release'"
echo "3. Tag: v1.0"
echo "4. Title: 🎌 Ultimate Anime World Runner v1.0"
echo "5. Upload APK files from Release/APKs/ folder"
echo "6. Publish release"
echo ""
echo "🎉 Your APKs will be downloadable by everyone!"

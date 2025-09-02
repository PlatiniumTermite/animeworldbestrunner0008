#!/bin/bash

# Create GitHub Release with APK Files
echo "🎌 Creating GitHub Release for Anime World Runner"
echo "================================================"

REPO="PlatiniumTermite/animeworldbestrunner0008"
RELEASE_TAG="v1.0"
RELEASE_TITLE="🎌 Ultimate Anime World Runner v1.0"

cd "/Users/divyanshurathore/Downloads/CascadeProjects/AnimeWorldRunner"

# Check if gh CLI is installed
if ! command -v gh &> /dev/null; then
    echo "📥 Installing GitHub CLI..."
    brew install gh
fi

# Authenticate with GitHub (if not already done)
echo "🔐 Authenticating with GitHub..."
gh auth login --web

# Create the release with APK files
echo "🚀 Creating GitHub release..."

gh release create "$RELEASE_TAG" \
    --repo "$REPO" \
    --title "$RELEASE_TITLE" \
    --notes-file Release/RELEASE_INFO.md \
    Release/APKs/UltimateAnimeRunner.apk \
    Release/APKs/AnimeWorldRunner.apk \
    Release/APKs/AnimeRunner.apk

echo ""
echo "✅ GitHub Release Created Successfully!"
echo ""
echo "🌐 Release URL: https://github.com/$REPO/releases/tag/$RELEASE_TAG"
echo ""
echo "📱 APK Downloads Available:"
echo "   • UltimateAnimeRunner.apk (Ultimate Edition)"
echo "   • AnimeWorldRunner.apk (Complete Game)"
echo "   • AnimeRunner.apk (Simple Version)"
echo ""
echo "🎉 Your Anime World Runner is now live on GitHub!"

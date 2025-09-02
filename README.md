# 🎌 Anime World Runner - Ultimate Edition

**The Best Anime-Style Mobile Runner Game** 🏃‍♀️✨

A complete Genshin Impact-inspired endless runner with advanced 3D movement, character customization, and mobile optimization. Features multiple APK builds, web deployment, and comprehensive Android support.

[![GitHub Release](https://img.shields.io/badge/Release-v1.0-brightgreen)](https://github.com/PlatiniumTermite/animeworldbestrunner0008)
[![Android](https://img.shields.io/badge/Android-21%2B-green)](https://developer.android.com)
[![Unreal Engine](https://img.shields.io/badge/Unreal-5.3%2B-blue)](https://unrealengine.com)

## 🎮 Ultimate Game Features

### 🎌 **Genshin Impact-Style Gameplay**
- **Full 3D movement system** with directional control (MoveForward/MoveRight)
- **Advanced character abilities**: Dash, Glide, Wall Climb, Combat
- **6 unlockable anime characters** with unique animations
- **Dynamic movement modes**: Walking, Running, Sprinting
- **Physics-based jumping** with air control and wall-jumping
- **Combat system** with attack and spell casting abilities

### 📱 **Mobile-Optimized Features**
- **Gesture-based touch controls** with swipe recognition
- **Combo scoring system** with multipliers
- **Real-time particle effects** and visual feedback
- **Character customization** with unlockable items and colors
- **Progressive level system** with auto-difficulty scaling
- **Performance optimization** for Samsung A56 and all Android devices

### 🚀 **Multiple Deployment Options**
- **Native APK files** (9 different builds available)
- **Web-based deployment** for instant compatibility
- **Android Studio emulator** support
- **Physical device installation** ready

### Mobile Optimization
- **Performance-optimized** for Samsung A56 and similar devices
- **Object pooling system** for efficient memory management
- **Dynamic resolution scaling** based on device performance
- **Device tier detection** for automatic graphics adjustment
- **Battery optimization mode** for extended gameplay
- **ASTC texture compression** for optimal mobile rendering

### Technical Features
- **Complete C++ implementation** with Blueprint integration
- **Save/load system** for persistent progress
- **UI system** with multiple screens (Menu, Game, Pause, Game Over)
- **Audio management** with volume controls
- **Character unlocking system**
- **High score tracking** and statistics

## 🏗️ Project Structure

```
AnimeWorldRunner/
├── Source/
│   └── AnimeWorldRunner/
│       ├── Public/
│       │   ├── AnimeRunnerCharacter.h          # Main player character
│       │   ├── GameModes/
│       │   │   └── AWRGameModeBase.h           # Game mode with scoring
│       │   ├── Actors/
│       │   │   ├── Obstacle.h                  # Obstacle system
│       │   │   └── Collectible.h               # Collectible items
│       │   ├── Components/
│       │   │   ├── InventoryComponent.h        # Player inventory
│       │   │   └── SaveGameComponent.h         # Save/load system
│       │   ├── UI/
│       │   │   └── GameHUD.h                   # UI management
│       │   └── Utilities/
│       │       ├── ObjectPool.h                # Performance optimization
│       │       └── AWRBlueprintLibrary.h       # Mobile utilities
│       └── Private/
│           └── [Implementation files]
├── Content/
│   ├── Characters/                             # Character assets
│   ├── Environments/                           # Environment pieces
│   ├── UI/                                     # User interface
│   ├── Blueprints/                             # Blueprint classes
│   ├── Materials/                              # Mobile-optimized materials
│   └── Audio/                                  # Sound effects and music
├── Config/
│   ├── DefaultEngine.ini                       # Engine configuration
│   ├── DefaultInput.ini                        # Input mappings
│   └── DefaultGame.ini                         # Game settings
└── README.md
```

## 🚀 Getting Started

### Prerequisites
- **Unreal Engine 5.3** or later
- **Visual Studio 2022** (Windows) or **Xcode** (Mac)
- **Android Studio** and **Android SDK/NDK** for mobile deployment
- **Git** for version control

### Setup Instructions

1. **Clone the Repository**
   ```bash
   git clone <repository-url>
   cd AnimeWorldRunner
   ```

2. **Generate Project Files**
   - Double-click `AnimeWorldRunner.uproject`
   - Click "Yes" when prompted to generate project files

3. **Compile the Project**
   - Open the generated `.sln` file in Visual Studio
   - Build the solution (Ctrl+Shift+B)
   - Or compile directly in Unreal Editor

4. **Configure Android Development**
   - Install Android Studio and configure SDK/NDK paths in Unreal Editor
   - Set up signing keys for release builds
   - Configure target Android API levels (21-30)

## 🎯 Core Systems

### Character Movement System
The `AAnimeRunnerCharacter` class provides:
- **Lane-based movement** with smooth transitions
- **Touch input handling** for mobile devices
- **State management** (Idle, Running, Jumping, Sliding, etc.)
- **Auto-run functionality** with progressive speed increase
- **Collision detection** for obstacles and collectibles

```cpp
// Example: Lane switching
void AAnimeRunnerCharacter::MoveRight(float Value)
{
    int32 NewLane = FMath::Clamp(CurrentLane + FMath::RoundToInt(Value), -1, 1);
    if (NewLane != CurrentLane) 
    {
        CurrentLane = NewLane;
        TargetPosition = FVector(GetActorLocation().X, CurrentLane * LaneWidth, GetActorLocation().Z);
    }
}
```

### Object Pooling System
Optimizes performance by reusing objects:
```cpp
// Get object from pool
AActor* PooledActor = ObjectPool->GetPooledObject();

// Return object to pool when done
ObjectPool->ReturnPooledObject(PooledActor);
```

### Mobile Optimization
The `UAWRBlueprintLibrary` provides utilities for:
- **Device performance detection**
- **Graphics quality adjustment**
- **Memory management**
- **Frame rate control**
- **Battery optimization**

```cpp
// Auto-adjust graphics based on device
UAWRBlueprintLibrary::AdjustGraphicsForDevice();

// Enable battery saving mode
UAWRBlueprintLibrary::EnableBatteryOptimization(true);
```

## 📱 Mobile Controls

### Touch Controls
- **Tap**: Jump
- **Swipe Left**: Move to left lane
- **Swipe Right**: Move to right lane
- **Swipe Down**: Slide (when implemented with gesture recognition)

### Keyboard Controls (Testing)
- **Space**: Jump
- **A/Left Arrow**: Move left
- **D/Right Arrow**: Move right
- **Ctrl**: Slide
- **E**: Use power-up
- **Escape**: Pause

## 🎨 Art Guidelines

### Mobile Optimization
- **Textures**: 1024x1024 or 512x512 maximum
- **Compression**: ASTC for Android, ETC2 fallback
- **Models**: Under 5k triangles for main characters
- **Materials**: Simple shaders with limited instructions

### Anime Style
- **Toon shading** with flat colors and sharp shadows
- **Bright, saturated colors** for visual appeal
- **Simple geometric shapes** for performance
- **Minimal post-processing** to maintain frame rate

## 🔧 Performance Optimization

### Graphics Settings
```ini
[/Script/Engine.RendererSettings]
r.Mobile.EnableStaticAndCSMShadowReceivers=True
r.DefaultFeature.MotionBlur=False
r.DefaultFeature.AmbientOcclusion=False
r.AntiAliasingMethod=1
r.MSAACount=4
```

### Device Profiles
- **Low-end**: 30 FPS, minimal effects, 50% resolution scale
- **Mid-range**: 60 FPS, moderate effects, 75% resolution scale
- **High-end**: 60 FPS, full effects, 100% resolution scale

## 💾 Save System

The game automatically saves:
- **High scores** and best distances
- **Total coins** collected
- **Unlocked characters** and power-ups
- **Player preferences** (sound, music, controls)
- **Statistics** (games played, achievements)

## 🚀 Deployment

### Android Build Configuration
1. **Package Settings**
   - Build Configuration: Shipping
   - Target SDK: API 30
   - Minimum SDK: API 21

2. **Optimization**
   - Enable Proguard for release builds
   - Use APK splitting for different architectures
   - Optimize texture formats (ASTC primary)

3. **Testing**
   - Test on various Android devices
   - Monitor performance with device profiler
   - Validate touch controls and UI scaling

## 🎵 Audio System

### Implementation
- **Sound effects** for actions (jump, collect, hit)
- **Background music** with loop support
- **Volume controls** in settings
- **Audio compression** optimized for mobile

### Integration
```cpp
// Play sound effect
UGameplayStatics::PlaySound2D(this, JumpSound);

// Control music volume
UAudioComponent* MusicComponent = GetAudioComponent();
MusicComponent->SetVolumeMultiplier(SaveGameComponent->GetMusicVolume());
```

## 🏆 Achievements & Progression

### Unlockable Content
- **Characters**: Unlock with coins or achievements
- **Power-ups**: Unlock through gameplay milestones
- **Environments**: Unlock with distance achievements

### Statistics Tracking
- **Total distance** traveled
- **Coins collected** across all games
- **Power-ups used** and effectiveness
- **Obstacles avoided** count

## 🐛 Debugging & Testing

### Performance Monitoring
```cpp
// Check current FPS
float CurrentFPS = UAWRBlueprintLibrary::GetCurrentFPS();

// Monitor memory usage
float MemoryMB = UAWRBlueprintLibrary::GetMemoryUsageMB();
```

### Common Issues
- **Frame rate drops**: Check object pooling, reduce draw calls
- **Memory leaks**: Ensure proper object cleanup
- **Touch responsiveness**: Verify input settings and UI scaling

## 📈 Future Enhancements

### Planned Features
- **Multiplayer racing** mode
- **Daily challenges** and events
- **Social features** (leaderboards, sharing)
- **More power-ups** and character abilities
- **Dynamic weather** and time-of-day system

### Technical Improvements
- **Procedural generation** for infinite variety
- **Advanced AI** for dynamic difficulty
- **Cloud save** synchronization
- **Analytics integration** for player behavior

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- **Unreal Engine** for the powerful game development framework
- **Epic Games** for mobile optimization guidelines
- **Community contributors** for feedback and testing

---

**Ready to run in the anime world!** 🏃‍♂️✨

For support or questions, contact: support@yourcompany.com

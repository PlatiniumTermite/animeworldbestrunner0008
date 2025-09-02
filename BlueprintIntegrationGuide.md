# Anime World Runner - Blueprint Integration Guide

This guide explains how to integrate the C++ systems we've created with Unreal Engine Blueprints for your Genshin Impact-style anime runner game.

## 🎯 Overview

We've created a complete anime-style asset pipeline with the following systems:
- **AnimeRunnerCharacter** - 3D movement and abilities
- **AnimeEffectsManager** - Particle and audio effects
- **AnimeCharacterAnimInstance** - Animation system
- **AnimeMaterialManager** - Cel-shading materials
- **ModularEnvironmentSystem** - Procedural world generation
- **AnimeUIManager** - UI system with anime-style animations
- **MobileOptimizationManager** - Performance optimization
- **CharacterCustomizationManager** - Character customization

## 📋 Blueprint Integration Steps

### 1. Character Setup

#### Create Character Blueprint
1. Create Blueprint based on `AAnimeRunnerCharacter`
2. Set up mesh components:
   ```
   - Main Character Mesh (SkeletalMeshComponent)
   - Hair Mesh (StaticMeshComponent) - Name: "HairMesh"
   - Accessory Meshes (StaticMeshComponent) - Name: "AccessoryMesh"
   - Weapon Mesh (StaticMeshComponent) - Name: "WeaponMesh"
   ```

#### Add Component References
```cpp
// In Blueprint Construction Script
AnimeEffectsManager = CreateDefaultSubobject<UAnimeEffectsManager>("EffectsManager");
AnimeMaterialManager = CreateDefaultSubobject<UAnimeMaterialManager>("MaterialManager");
CharacterCustomizationManager = CreateDefaultSubobject<UCharacterCustomizationManager>("CustomizationManager");
```

### 2. Animation Blueprint

#### Create Animation Blueprint
1. Parent Class: `UAnimeCharacterAnimInstance`
2. Set up State Machine with states:
   - **Idle** - Standing still
   - **Walking** - Normal movement
   - **Running** - Fast movement
   - **Jumping** - In air
   - **Gliding** - Glide state
   - **Climbing** - Wall climbing
   - **Attacking** - Combat animations
   - **Casting** - Spell casting

#### Animation Variables (Auto-populated from C++)
```cpp
// Movement
float Speed;
float Direction;
bool bIsInAir;
bool bIsMoving;

// States
bool bIsGliding;
bool bIsClimbing;
bool bIsAttacking;
bool bIsCasting;

// IK and Facial
FVector LeftFootIKLocation;
FVector RightFootIKLocation;
float BlinkAlpha;
FVector2D EyeLookDirection;
```

### 3. Material Setup

#### Create Master Material
1. Create Material: `M_AnimeCelShading_Master`
2. Add parameters:
   ```
   - BaseColor (Vector3)
   - RimLightColor (Vector3)
   - RimLightPower (Scalar)
   - OutlineWidth (Scalar)
   - ShadowColor (Vector3)
   - SpecularColor (Vector3)
   - Roughness (Scalar)
   - Metallic (Scalar)
   ```

#### Material Instances
Create instances for each category:
- `MI_Character_Hair`
- `MI_Character_Skin`
- `MI_Character_Eyes`
- `MI_Character_Outfit`
- `MI_Environment_Ground`
- `MI_Environment_Foliage`

### 4. Effects System

#### Particle Systems
Create particle systems for:
```
- P_Dash_Trail - Dash effect trail
- P_Jump_Burst - Jump landing effect
- P_Run_Dust - Running dust particles
- P_Elemental_Burst - Special ability effects
- P_Impact_Sparks - Combat impact effects
- P_Aura_Glow - Character aura effects
```

#### Audio Cues
Create sound cues for:
```
- SC_Footstep_Grass
- SC_Footstep_Stone
- SC_Jump_Launch
- SC_Jump_Land
- SC_Dash_Whoosh
- SC_Attack_Swing
- SC_Spell_Cast
- SC_UI_Click
- SC_UI_Hover
```

### 5. UI System

#### Widget Blueprints
Create widgets based on `AnimeUIManager`:

**Main HUD (WBP_GameHUD)**
```cpp
// Bind to C++ functions
UpdateScore(int32 Score);
UpdateCoins(int32 Coins);
UpdateDistance(float Distance);
UpdateHealth(float Health, float MaxHealth);
```

**Main Menu (WBP_MainMenu)**
- Play Button
- Settings Button
- Character Customization Button
- Exit Button

**Character Customization (WBP_CharacterCustomization)**
```cpp
// Category buttons
OnHairButtonClicked() -> ShowItemsByCategory(ECustomizationCategory::Hair)
OnOutfitButtonClicked() -> ShowItemsByCategory(ECustomizationCategory::Outfit)

// Item selection
OnItemSelected(FString ItemID) -> PreviewItem(Category, ItemID)
OnItemConfirmed() -> EquipItem(Category, ItemID)

// Color picker
OnColorChanged(FLinearColor Color) -> SetItemColor(Category, Color)
```

### 6. Environment System

#### Environment Pieces
Create static meshes for:
```
- SM_Ground_Grass_01
- SM_Platform_Stone_01
- SM_Tree_Pine_01
- SM_Rock_Boulder_01
- SM_Building_House_01
- SM_Bridge_Wood_01
```

#### Environment Themes
Set up data tables:

**DT_EnvironmentPieces** (Based on `FCustomizationItem`)
```
ItemID | Mesh | Materials | Category | SpawnWeight
ground_grass | SM_Ground_Grass_01 | [MI_Ground_Grass] | Ground | 1.0
platform_stone | SM_Platform_Stone_01 | [MI_Platform_Stone] | Platform | 0.8
tree_pine | SM_Tree_Pine_01 | [MI_Tree_Pine] | Tree | 0.6
```

### 7. Mobile Optimization

#### Device Profiles
Create device profiles in:
`Config/DeviceProfiles/Android_Low.ini`
```ini
[Android_Low DeviceProfile]
DeviceType=Android
BaseProfileName=Android
+CVars=r.MaterialQualityLevel=0
+CVars=r.ShadowQuality=0
+CVars=r.PostProcessAAQuality=0
+CVars=r.BloomQuality=0
```

#### Quality Settings Blueprint
Create `BP_QualityManager`:
```cpp
// Auto-detect device and apply settings
BeginPlay() -> MobileOptimizationManager->AutoDetectQualityLevel()

// Manual quality adjustment
OnQualityChanged(EMobileQualityLevel Quality) -> SetQualityLevel(Quality)
```

### 8. Save System

#### Save Game Blueprint
Create `BP_CustomizationSaveGame` based on `USaveGame`:
```cpp
// Save data structure
struct FPlayerSaveData
{
    FPlayerCustomization CharacterCustomization;
    int32 PlayerLevel;
    int32 Coins;
    TArray<FString> UnlockedAchievements;
    float HighScore;
}
```

## 🎨 Asset Creation Guidelines

### Textures
- **Resolution**: 512x512 for characters, 256x256 for environment
- **Format**: ASTC for mobile, BC7 for desktop
- **Channels**: 
  - Diffuse: RGB (Color) + A (Opacity)
  - Normal: RG (Normal) + B (Roughness) + A (Metallic)

### Meshes
- **Character**: <5000 triangles
- **Environment pieces**: <2000 triangles each
- **LODs**: Create 3 LOD levels (100%, 50%, 25% detail)

### Materials
- Use mobile-friendly shaders
- Limit texture samples to 8 per material
- Use vertex colors for variation
- Implement proper LOD transitions

## 🚀 Performance Tips

### Rendering
1. Use instanced static meshes for repeated objects
2. Enable occlusion culling
3. Set appropriate draw distances
4. Use texture streaming

### Memory
1. Compress textures with ASTC
2. Use object pooling for effects
3. Stream audio files
4. Limit particle counts

### CPU
1. Use timers instead of Tick when possible
2. Cache frequently accessed components
3. Use Blueprint nativization for performance-critical blueprints

## 🎯 Testing Checklist

### Functionality
- [ ] Character movement (WASD + mouse)
- [ ] All abilities work (Jump, Dash, Glide, Climb, Attack, Cast)
- [ ] UI responds to input
- [ ] Customization saves/loads
- [ ] Environment generates properly

### Performance (Samsung A56 Target)
- [ ] Maintains 60 FPS in gameplay
- [ ] Memory usage <2GB
- [ ] Load times <5 seconds
- [ ] No frame drops during effects

### Visual Quality
- [ ] Cel-shading looks anime-style
- [ ] Particle effects are smooth
- [ ] UI animations are fluid
- [ ] Environment has proper depth

## 📱 Mobile-Specific Setup

### Touch Controls
1. Enable virtual joystick in `AnimeUIManager`
2. Create touch button widgets
3. Set up gesture recognition for special moves

### Android Packaging
1. Set minimum SDK to 23 (Android 6.0)
2. Target SDK 33 (Android 13)
3. Enable ARM64 architecture
4. Use Universal APK for distribution

This integration guide provides the foundation for creating your Genshin Impact-style anime runner. The C++ systems handle the core functionality while Blueprints provide the visual and gameplay scripting layer.

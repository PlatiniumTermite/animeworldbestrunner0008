#include "Optimization/MobileOptimizationManager.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "HAL/PlatformApplicationMisc.h"
#include "Materials/MaterialParameterCollection.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "Engine/StaticMesh.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystem.h"
#include "RenderingThread.h"
#include "Stats/Stats.h"

UMobileOptimizationManager::UMobileOptimizationManager()
{
    PrimaryComponentTick.bCanEverTick = true;
    
    // Initialize default settings
    CurrentSettings = FMobileOptimizationSettings();
    bEnablePerformanceMonitoring = true;
    PerformanceUpdateInterval = 1.0f;
    
    bDynamicResolutionEnabled = false;
    CurrentResolutionScale = 1.0f;
    ResolutionAdjustmentSpeed = 0.1f;
    
    PerformanceTimer = 0.0f;
    FrameCounter = 0;
    
    FPSHistory.Reserve(60); // Store 60 frames of history
}

void UMobileOptimizationManager::BeginPlay()
{
    Super::BeginPlay();
    
    InitializeDeviceProfiles();
    AutoDetectQualityLevel();
    ApplyOptimizationSettings(CurrentSettings);
    
    if (bEnablePerformanceMonitoring)
    {
        StartPerformanceMonitoring();
    }
}

void UMobileOptimizationManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    
    if (bEnablePerformanceMonitoring)
    {
        UpdatePerformanceMetrics(DeltaTime);
        
        if (bDynamicResolutionEnabled)
        {
            AdjustDynamicResolution();
        }
    }
}

void UMobileOptimizationManager::InitializeDeviceProfiles()
{
    // Samsung A56 Profile (Target Device)
    FMobileOptimizationSettings A56Profile;
    A56Profile.QualityLevel = EMobileQualityLevel::Medium;
    A56Profile.TextureQuality = EMobileTextureQuality::Half;
    A56Profile.bEnableDynamicResolution = true;
    A56Profile.TargetFrameRate = 60.0f;
    A56Profile.MinResolutionScale = 0.6f;
    A56Profile.MaxResolutionScale = 1.0f;
    A56Profile.LODBias = 1.0f;
    A56Profile.MaxDrawDistance = 2500.0f;
    A56Profile.bEnableShadows = true;
    A56Profile.ShadowDistance = 800.0f;
    A56Profile.ShadowMapResolution = 512;
    A56Profile.MaxDrawCalls = 80;
    A56Profile.MaxTriangles = 80000;
    DeviceProfiles.Add("SM-A546B", A56Profile); // Samsung A56 model number
    
    // Low-end device profile
    FMobileOptimizationSettings LowEndProfile;
    LowEndProfile.QualityLevel = EMobileQualityLevel::Low;
    LowEndProfile.TextureQuality = EMobileTextureQuality::Quarter;
    LowEndProfile.bEnableDynamicResolution = true;
    LowEndProfile.TargetFrameRate = 30.0f;
    LowEndProfile.MinResolutionScale = 0.4f;
    LowEndProfile.MaxResolutionScale = 0.8f;
    LowEndProfile.LODBias = 2.0f;
    LowEndProfile.MaxDrawDistance = 1500.0f;
    LowEndProfile.bEnableShadows = false;
    LowEndProfile.bEnablePostProcessing = false;
    LowEndProfile.MaxDrawCalls = 50;
    LowEndProfile.MaxTriangles = 40000;
    DeviceProfiles.Add("LowEnd", LowEndProfile);
    
    // High-end device profile
    FMobileOptimizationSettings HighEndProfile;
    HighEndProfile.QualityLevel = EMobileQualityLevel::High;
    HighEndProfile.TextureQuality = EMobileTextureQuality::Full;
    HighEndProfile.bEnableDynamicResolution = false;
    HighEndProfile.TargetFrameRate = 60.0f;
    HighEndProfile.MinResolutionScale = 0.8f;
    HighEndProfile.MaxResolutionScale = 1.2f;
    HighEndProfile.LODBias = 0.5f;
    HighEndProfile.MaxDrawDistance = 4000.0f;
    HighEndProfile.bEnableShadows = true;
    HighEndProfile.ShadowDistance = 1500.0f;
    HighEndProfile.ShadowMapResolution = 1024;
    HighEndProfile.bEnableBloom = true;
    HighEndProfile.bEnableAntiAliasing = true;
    HighEndProfile.MaxDrawCalls = 150;
    HighEndProfile.MaxTriangles = 150000;
    DeviceProfiles.Add("HighEnd", HighEndProfile);
}

void UMobileOptimizationManager::ApplyOptimizationSettings(const FMobileOptimizationSettings& Settings)
{
    CurrentSettings = Settings;
    
    // Apply quality level
    ApplyQualitySettings(Settings.QualityLevel);
    
    // Set console variables for optimization
    SetConsoleVariables();
    
    // Apply texture quality
    SetTextureQuality(Settings.TextureQuality);
    
    // Apply LOD settings
    SetLODBias(Settings.LODBias);
    SetMaxDrawDistance(Settings.MaxDrawDistance);
    
    // Apply dynamic resolution
    EnableDynamicResolution(Settings.bEnableDynamicResolution);
    SetTargetFrameRate(Settings.TargetFrameRate);
    
    // Update material parameter collection
    if (OptimizationMPC)
    {
        UMaterialParameterCollectionInstance* MPCInstance = GetWorld()->GetParameterCollectionInstance(OptimizationMPC);
        if (MPCInstance)
        {
            MPCInstance->SetScalarParameterValue("QualityLevel", (float)Settings.QualityLevel);
            MPCInstance->SetScalarParameterValue("LODBias", Settings.LODBias);
            MPCInstance->SetScalarParameterValue("MaxDrawDistance", Settings.MaxDrawDistance);
            MPCInstance->SetScalarParameterValue("ShadowQuality", Settings.bEnableShadows ? 1.0f : 0.0f);
        }
    }
}

void UMobileOptimizationManager::SetQualityLevel(EMobileQualityLevel QualityLevel)
{
    CurrentSettings.QualityLevel = QualityLevel;
    ApplyQualitySettings(QualityLevel);
}

void UMobileOptimizationManager::ApplyQualitySettings(EMobileQualityLevel QualityLevel)
{
    switch (QualityLevel)
    {
        case EMobileQualityLevel::Low:
            OptimizeForLowEnd();
            break;
        case EMobileQualityLevel::Medium:
            OptimizeForMidRange();
            break;
        case EMobileQualityLevel::High:
        case EMobileQualityLevel::Ultra:
            OptimizeForHighEnd();
            break;
        case EMobileQualityLevel::Auto:
            AutoDetectQualityLevel();
            break;
    }
}

void UMobileOptimizationManager::AutoDetectQualityLevel()
{
    FString DeviceModel = DetectDeviceModel();
    
    // Check if we have a specific profile for this device
    if (FMobileOptimizationSettings* DeviceProfile = DeviceProfiles.Find(DeviceModel))
    {
        ApplyOptimizationSettings(*DeviceProfile);
        return;
    }
    
    // Fallback to performance-based detection
    float AverageFPS = CurrentMetrics.AverageFPS;
    
    if (AverageFPS >= 55.0f)
    {
        SetQualityLevel(EMobileQualityLevel::High);
    }
    else if (AverageFPS >= 45.0f)
    {
        SetQualityLevel(EMobileQualityLevel::Medium);
    }
    else
    {
        SetQualityLevel(EMobileQualityLevel::Low);
    }
}

FString UMobileOptimizationManager::DetectDeviceModel()
{
    // Get device model - this is platform specific
    FString DeviceModel = FPlatformMisc::GetDeviceId();
    
    // For Samsung A56, we would expect something like "SM-A546B"
    if (DeviceModel.Contains("SM-A546") || DeviceModel.Contains("A56"))
    {
        return "SM-A546B";
    }
    
    // Fallback detection based on performance characteristics
    return "Unknown";
}

void UMobileOptimizationManager::OptimizeForDevice(const FString& DeviceName)
{
    if (FMobileOptimizationSettings* DeviceProfile = DeviceProfiles.Find(DeviceName))
    {
        ApplyOptimizationSettings(*DeviceProfile);
    }
    else
    {
        // Default to medium quality for unknown devices
        SetQualityLevel(EMobileQualityLevel::Medium);
    }
}

void UMobileOptimizationManager::EnableDynamicResolution(bool bEnable)
{
    bDynamicResolutionEnabled = bEnable;
    CurrentSettings.bEnableDynamicResolution = bEnable;
    
    if (bEnable)
    {
        // Enable dynamic resolution scaling
        if (GEngine)
        {
            GEngine->Exec(GetWorld(), TEXT("r.DynamicRes.OperationMode 1"));
            GEngine->Exec(GetWorld(), *FString::Printf(TEXT("r.DynamicRes.MinResolutionScale %f"), CurrentSettings.MinResolutionScale));
            GEngine->Exec(GetWorld(), *FString::Printf(TEXT("r.DynamicRes.MaxResolutionScale %f"), CurrentSettings.MaxResolutionScale));
        }
    }
    else
    {
        if (GEngine)
        {
            GEngine->Exec(GetWorld(), TEXT("r.DynamicRes.OperationMode 0"));
        }
    }
}

void UMobileOptimizationManager::SetResolutionScale(float Scale)
{
    CurrentResolutionScale = FMath::Clamp(Scale, CurrentSettings.MinResolutionScale, CurrentSettings.MaxResolutionScale);
    
    if (GEngine)
    {
        GEngine->Exec(GetWorld(), *FString::Printf(TEXT("r.ScreenPercentage %f"), CurrentResolutionScale * 100.0f));
    }
}

void UMobileOptimizationManager::SetTargetFrameRate(float TargetFPS)
{
    CurrentSettings.TargetFrameRate = TargetFPS;
    
    if (GEngine)
    {
        GEngine->Exec(GetWorld(), *FString::Printf(TEXT("t.MaxFPS %f"), TargetFPS));
    }
}

void UMobileOptimizationManager::SetTextureQuality(EMobileTextureQuality Quality)
{
    CurrentSettings.TextureQuality = Quality;
    
    float TextureScale = 1.0f;
    switch (Quality)
    {
        case EMobileTextureQuality::Quarter:
            TextureScale = 0.25f;
            break;
        case EMobileTextureQuality::Half:
            TextureScale = 0.5f;
            break;
        case EMobileTextureQuality::Full:
            TextureScale = 1.0f;
            break;
        case EMobileTextureQuality::Native:
            TextureScale = 1.0f;
            break;
    }
    
    if (GEngine)
    {
        GEngine->Exec(GetWorld(), *FString::Printf(TEXT("r.Streaming.MipBias %f"), FMath::Log2(1.0f / TextureScale)));
    }
}

void UMobileOptimizationManager::SetLODBias(float Bias)
{
    CurrentSettings.LODBias = Bias;
    
    if (GEngine)
    {
        GEngine->Exec(GetWorld(), *FString::Printf(TEXT("r.StaticMeshLODBias %f"), Bias));
        GEngine->Exec(GetWorld(), *FString::Printf(TEXT("r.SkeletalMeshLODBias %f"), Bias));
    }
}

void UMobileOptimizationManager::SetMaxDrawDistance(float Distance)
{
    CurrentSettings.MaxDrawDistance = Distance;
    
    if (GEngine)
    {
        GEngine->Exec(GetWorld(), *FString::Printf(TEXT("r.DrawDistanceScale %f"), Distance / 5000.0f));
    }
}

void UMobileOptimizationManager::UpdatePerformanceMetrics(float DeltaTime)
{
    PerformanceTimer += DeltaTime;
    FrameCounter++;
    
    // Update FPS
    if (DeltaTime > 0.0f)
    {
        CurrentMetrics.CurrentFPS = 1.0f / DeltaTime;
        CurrentMetrics.FrameTime = DeltaTime * 1000.0f; // Convert to milliseconds
        
        // Add to history
        FPSHistory.Add(CurrentMetrics.CurrentFPS);
        if (FPSHistory.Num() > 60)
        {
            FPSHistory.RemoveAt(0);
        }
        
        // Calculate average FPS
        float TotalFPS = 0.0f;
        for (float FPS : FPSHistory)
        {
            TotalFPS += FPS;
        }
        CurrentMetrics.AverageFPS = TotalFPS / FPSHistory.Num();
    }
    
    // Update other metrics periodically
    if (PerformanceTimer >= PerformanceUpdateInterval)
    {
        // Get rendering stats (simplified - would need proper stat gathering in production)
        CurrentMetrics.DrawCalls = 0; // Would get from rendering stats
        CurrentMetrics.Triangles = 0; // Would get from rendering stats
        CurrentMetrics.GPUTime = 0.0f; // Would get from GPU profiling
        CurrentMetrics.CPUTime = CurrentMetrics.FrameTime;
        CurrentMetrics.MemoryUsage = GetMemoryUsage();
        CurrentMetrics.CurrentResolutionScale = CurrentResolutionScale;
        
        PerformanceTimer = 0.0f;
    }
}

void UMobileOptimizationManager::AdjustDynamicResolution()
{
    if (!bDynamicResolutionEnabled) return;
    
    float TargetFPS = CurrentSettings.TargetFrameRate;
    float CurrentFPS = CurrentMetrics.CurrentFPS;
    float FPSDifference = CurrentFPS - TargetFPS;
    
    // Adjust resolution based on FPS performance
    if (FPSDifference < -5.0f) // FPS too low
    {
        float NewScale = CurrentResolutionScale - (ResolutionAdjustmentSpeed * 0.1f);
        SetResolutionScale(NewScale);
    }
    else if (FPSDifference > 5.0f) // FPS too high, can increase quality
    {
        float NewScale = CurrentResolutionScale + (ResolutionAdjustmentSpeed * 0.05f);
        SetResolutionScale(NewScale);
    }
}

FPerformanceMetrics UMobileOptimizationManager::GetPerformanceMetrics()
{
    return CurrentMetrics;
}

bool UMobileOptimizationManager::IsPerformanceTargetMet()
{
    return CurrentMetrics.AverageFPS >= (CurrentSettings.TargetFrameRate * 0.9f); // 90% of target
}

void UMobileOptimizationManager::StartPerformanceMonitoring()
{
    bEnablePerformanceMonitoring = true;
    FPSHistory.Empty();
    PerformanceTimer = 0.0f;
    FrameCounter = 0;
}

void UMobileOptimizationManager::StopPerformanceMonitoring()
{
    bEnablePerformanceMonitoring = false;
}

float UMobileOptimizationManager::GetMemoryUsage()
{
    // Simplified memory usage calculation
    FPlatformMemoryStats MemStats = FPlatformMemory::GetStats();
    return MemStats.UsedPhysical / (1024.0f * 1024.0f); // Convert to MB
}

void UMobileOptimizationManager::OptimizeMemoryUsage()
{
    // Force garbage collection
    GarbageCollect();
    
    // Reduce texture streaming pool
    if (GEngine)
    {
        GEngine->Exec(GetWorld(), TEXT("r.Streaming.PoolSize 512"));
    }
}

void UMobileOptimizationManager::GarbageCollect()
{
    if (GEngine)
    {
        GEngine->ForceGarbageCollection(true);
    }
}

void UMobileOptimizationManager::SetConsoleVariables()
{
    if (!GEngine) return;
    
    // Mobile-specific optimizations
    GEngine->Exec(GetWorld(), TEXT("r.Mobile.EnableStaticAndCSMShadowReceivers 1"));
    GEngine->Exec(GetWorld(), TEXT("r.Mobile.EnableMovableLightCSMShaderCulling 1"));
    GEngine->Exec(GetWorld(), TEXT("r.Mobile.AllowDitheredLODTransition 1"));
    
    // Occlusion culling
    if (CurrentSettings.bEnableOcclusion)
    {
        GEngine->Exec(GetWorld(), TEXT("r.HZBOcclusion 1"));
        GEngine->Exec(GetWorld(), TEXT("r.OcclusionCulling 1"));
    }
    else
    {
        GEngine->Exec(GetWorld(), TEXT("r.HZBOcclusion 0"));
        GEngine->Exec(GetWorld(), TEXT("r.OcclusionCulling 0"));
    }
    
    // Shadow settings
    if (CurrentSettings.bEnableShadows)
    {
        GEngine->Exec(GetWorld(), *FString::Printf(TEXT("r.Shadow.MaxResolution %d"), CurrentSettings.ShadowMapResolution));
        GEngine->Exec(GetWorld(), *FString::Printf(TEXT("r.Shadow.DistanceScale %f"), CurrentSettings.ShadowDistance / 1000.0f));
    }
    else
    {
        GEngine->Exec(GetWorld(), TEXT("r.Shadow.MaxResolution 0"));
    }
    
    // Post processing
    if (!CurrentSettings.bEnablePostProcessing)
    {
        GEngine->Exec(GetWorld(), TEXT("r.PostProcessAAQuality 0"));
        GEngine->Exec(GetWorld(), TEXT("r.BloomQuality 0"));
    }
    else
    {
        if (CurrentSettings.bEnableAntiAliasing)
        {
            GEngine->Exec(GetWorld(), TEXT("r.PostProcessAAQuality 2"));
        }
        if (CurrentSettings.bEnableBloom)
        {
            GEngine->Exec(GetWorld(), TEXT("r.BloomQuality 2"));
        }
    }
}

void UMobileOptimizationManager::OptimizeForLowEnd()
{
    if (!GEngine) return;
    
    // Aggressive optimizations for low-end devices
    GEngine->Exec(GetWorld(), TEXT("r.MaterialQualityLevel 0"));
    GEngine->Exec(GetWorld(), TEXT("r.ShadowQuality 0"));
    GEngine->Exec(GetWorld(), TEXT("r.PostProcessAAQuality 0"));
    GEngine->Exec(GetWorld(), TEXT("r.BloomQuality 0"));
    GEngine->Exec(GetWorld(), TEXT("r.LightShaftQuality 0"));
    GEngine->Exec(GetWorld(), TEXT("r.RefractionQuality 0"));
    GEngine->Exec(GetWorld(), TEXT("r.SSR.Quality 0"));
    GEngine->Exec(GetWorld(), TEXT("r.DetailMode 0"));
    GEngine->Exec(GetWorld(), TEXT("r.TranslucencyLightingVolume 0"));
    
    // Reduce draw distance and LOD bias
    SetLODBias(2.0f);
    SetMaxDrawDistance(1500.0f);
    
    // Disable expensive features
    GEngine->Exec(GetWorld(), TEXT("fx.MaxGPUParticlesSpawnedPerFrame 64"));
    GEngine->Exec(GetWorld(), TEXT("r.Streaming.MipBias 2"));
}

void UMobileOptimizationManager::OptimizeForMidRange()
{
    if (!GEngine) return;
    
    // Balanced settings for mid-range devices like Samsung A56
    GEngine->Exec(GetWorld(), TEXT("r.MaterialQualityLevel 1"));
    GEngine->Exec(GetWorld(), TEXT("r.ShadowQuality 2"));
    GEngine->Exec(GetWorld(), TEXT("r.PostProcessAAQuality 1"));
    GEngine->Exec(GetWorld(), TEXT("r.BloomQuality 1"));
    GEngine->Exec(GetWorld(), TEXT("r.LightShaftQuality 1"));
    GEngine->Exec(GetWorld(), TEXT("r.RefractionQuality 1"));
    GEngine->Exec(GetWorld(), TEXT("r.SSR.Quality 1"));
    GEngine->Exec(GetWorld(), TEXT("r.DetailMode 1"));
    
    SetLODBias(1.0f);
    SetMaxDrawDistance(2500.0f);
    
    GEngine->Exec(GetWorld(), TEXT("fx.MaxGPUParticlesSpawnedPerFrame 128"));
    GEngine->Exec(GetWorld(), TEXT("r.Streaming.MipBias 1"));
}

void UMobileOptimizationManager::OptimizeForHighEnd()
{
    if (!GEngine) return;
    
    // High quality settings for premium devices
    GEngine->Exec(GetWorld(), TEXT("r.MaterialQualityLevel 2"));
    GEngine->Exec(GetWorld(), TEXT("r.ShadowQuality 3"));
    GEngine->Exec(GetWorld(), TEXT("r.PostProcessAAQuality 2"));
    GEngine->Exec(GetWorld(), TEXT("r.BloomQuality 3"));
    GEngine->Exec(GetWorld(), TEXT("r.LightShaftQuality 2"));
    GEngine->Exec(GetWorld(), TEXT("r.RefractionQuality 2"));
    GEngine->Exec(GetWorld(), TEXT("r.SSR.Quality 2"));
    GEngine->Exec(GetWorld(), TEXT("r.DetailMode 2"));
    
    SetLODBias(0.5f);
    SetMaxDrawDistance(4000.0f);
    
    GEngine->Exec(GetWorld(), TEXT("fx.MaxGPUParticlesSpawnedPerFrame 256"));
    GEngine->Exec(GetWorld(), TEXT("r.Streaming.MipBias 0"));
}

void UMobileOptimizationManager::OptimizeRenderingSettings()
{
    ApplyOptimizationSettings(CurrentSettings);
}

void UMobileOptimizationManager::SetMaxDrawCalls(int32 MaxCalls)
{
    CurrentSettings.MaxDrawCalls = MaxCalls;
    // This would typically be enforced at the rendering level
}

void UMobileOptimizationManager::EnableInstancing(bool bEnable)
{
    CurrentSettings.bEnableInstancing = bEnable;
    
    if (GEngine)
    {
        GEngine->Exec(GetWorld(), bEnable ? TEXT("r.InstancedStereo 1") : TEXT("r.InstancedStereo 0"));
    }
}

void UMobileOptimizationManager::CompressTextures()
{
    // Enable ASTC texture compression for mobile
    if (GEngine)
    {
        GEngine->Exec(GetWorld(), TEXT("r.Mobile.UseHWsRGBEncoding 1"));
    }
}

void UMobileOptimizationManager::StreamTextures(bool bEnable)
{
    if (GEngine)
    {
        GEngine->Exec(GetWorld(), bEnable ? TEXT("r.TextureStreaming 1") : TEXT("r.TextureStreaming 0"));
    }
}

void UMobileOptimizationManager::UpdateLODSettings()
{
    SetLODBias(CurrentSettings.LODBias);
    SetMaxDrawDistance(CurrentSettings.MaxDrawDistance);
}

void UMobileOptimizationManager::OptimizeStaticMeshes()
{
    // This would iterate through static meshes and optimize them
    // Implementation would depend on specific optimization needs
}

void UMobileOptimizationManager::OptimizeMaterials()
{
    // This would optimize material complexity for mobile
    // Implementation would work with the AnimeMaterialManager
}

void UMobileOptimizationManager::OptimizeParticleSystems()
{
    if (GEngine)
    {
        // Reduce particle counts for mobile
        GEngine->Exec(GetWorld(), TEXT("fx.MaxGPUParticlesSpawnedPerFrame 64"));
        GEngine->Exec(GetWorld(), TEXT("fx.MaxCPUParticlesPerEmitter 100"));
    }
}

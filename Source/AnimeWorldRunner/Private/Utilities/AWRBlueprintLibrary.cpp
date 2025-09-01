#include "Utilities/AWRBlueprintLibrary.h"
#include "Engine/Engine.h"
#include "GameFramework/GameUserSettings.h"
#include "HAL/PlatformApplicationMisc.h"
#include "GenericPlatform/GenericPlatformMemory.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"

void UAWRBlueprintLibrary::AdjustGraphicsForDevice()
{
    UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings();
    if (!Settings)
    {
        return;
    }
    
    int32 DeviceTier = GetDevicePerformanceTier();
    
    switch (DeviceTier)
    {
        case 0: // Low-end device
            Settings->SetOverallScalabilityLevel(0);
            Settings->SetResolutionScaleValueEx(50.0f);
            SetTargetFramerate(30);
            break;
        case 1: // Mid-range device
            Settings->SetOverallScalabilityLevel(1);
            Settings->SetResolutionScaleValueEx(75.0f);
            SetTargetFramerate(60);
            break;
        case 2: // High-end device
            Settings->SetOverallScalabilityLevel(2);
            Settings->SetResolutionScaleValueEx(100.0f);
            SetTargetFramerate(60);
            break;
        default:
            Settings->SetOverallScalabilityLevel(1);
            break;
    }
    
    Settings->ApplySettings(false);
}

void UAWRBlueprintLibrary::SetTextureStreaming(bool Enable)
{
    if (GEngine)
    {
        GEngine->Exec(nullptr, Enable ? TEXT("r.TextureStreaming 1") : TEXT("r.TextureStreaming 0"));
    }
}

void UAWRBlueprintLibrary::SetDynamicResolution(bool Enable)
{
    if (GEngine)
    {
        GEngine->Exec(nullptr, Enable ? TEXT("r.DynamicRes.OperationMode 1") : TEXT("r.DynamicRes.OperationMode 0"));
    }
}

void UAWRBlueprintLibrary::PreloadAssets(const TArray<FSoftObjectPath>& Assets)
{
    if (UAssetManager* AssetManager = UAssetManager::GetIfValid())
    {
        FStreamableManager& StreamableManager = AssetManager->GetStreamableManager();
        StreamableManager.RequestAsyncLoad(Assets);
    }
}

void UAWRBlueprintLibrary::ClearUnusedAssets()
{
    if (GEngine)
    {
        GEngine->ForceGarbageCollection(true);
        GEngine->Exec(nullptr, TEXT("obj gc"));
    }
}

void UAWRBlueprintLibrary::SetTargetFramerate(int32 FrameRate)
{
    if (GEngine)
    {
        FString Command = FString::Printf(TEXT("t.MaxFPS %d"), FrameRate);
        GEngine->Exec(nullptr, *Command);
    }
}

int32 UAWRBlueprintLibrary::GetDevicePerformanceTier()
{
    // Simple device tier detection based on memory
    int32 MemoryMB = GetDeviceMemoryMB();
    
    if (MemoryMB < 2048) // Less than 2GB
    {
        return 0; // Low-end
    }
    else if (MemoryMB < 4096) // Less than 4GB
    {
        return 1; // Mid-range
    }
    else
    {
        return 2; // High-end
    }
}

FVector2D UAWRBlueprintLibrary::GetScreenResolution()
{
    if (GEngine && GEngine->GameViewport)
    {
        FVector2D ViewportSize;
        GEngine->GameViewport->GetViewportSize(ViewportSize);
        return ViewportSize;
    }
    return FVector2D(1920, 1080); // Default fallback
}

void UAWRBlueprintLibrary::SetScreenResolution(int32 Width, int32 Height)
{
    UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings();
    if (Settings)
    {
        Settings->SetScreenResolution(FIntPoint(Width, Height));
        Settings->ApplySettings(false);
    }
}

void UAWRBlueprintLibrary::SetGraphicsQuality(int32 QualityLevel)
{
    UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings();
    if (Settings)
    {
        Settings->SetOverallScalabilityLevel(FMath::Clamp(QualityLevel, 0, 3));
        Settings->ApplySettings(false);
    }
}

void UAWRBlueprintLibrary::SetShadowQuality(int32 QualityLevel)
{
    UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings();
    if (Settings)
    {
        Settings->SetShadowQuality(FMath::Clamp(QualityLevel, 0, 3));
        Settings->ApplySettings(false);
    }
}

void UAWRBlueprintLibrary::SetTextureQuality(int32 QualityLevel)
{
    UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings();
    if (Settings)
    {
        Settings->SetTextureQuality(FMath::Clamp(QualityLevel, 0, 3));
        Settings->ApplySettings(false);
    }
}

void UAWRBlueprintLibrary::SetEffectsQuality(int32 QualityLevel)
{
    UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings();
    if (Settings)
    {
        Settings->SetVisualEffectQuality(FMath::Clamp(QualityLevel, 0, 3));
        Settings->ApplySettings(false);
    }
}

void UAWRBlueprintLibrary::EnableBatteryOptimization(bool Enable)
{
    if (Enable)
    {
        SetTargetFramerate(30);
        SetGraphicsQuality(0);
        SetDynamicResolution(true);
    }
    else
    {
        SetTargetFramerate(60);
        AdjustGraphicsForDevice();
    }
}

FString UAWRBlueprintLibrary::GetDeviceModel()
{
    return FPlatformMisc::GetDeviceId();
}

int32 UAWRBlueprintLibrary::GetDeviceMemoryMB()
{
    FPlatformMemoryStats MemStats = FPlatformMemory::GetStats();
    return static_cast<int32>(MemStats.TotalPhysical / (1024 * 1024));
}

bool UAWRBlueprintLibrary::IsLowEndDevice()
{
    return GetDevicePerformanceTier() == 0;
}

float UAWRBlueprintLibrary::GetCurrentFPS()
{
    if (GEngine)
    {
        return GEngine->GetMaxFPS();
    }
    return 60.0f;
}

float UAWRBlueprintLibrary::GetMemoryUsageMB()
{
    FPlatformMemoryStats MemStats = FPlatformMemory::GetStats();
    return static_cast<float>(MemStats.UsedPhysical / (1024 * 1024));
}

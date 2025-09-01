#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/Engine.h"
#include "AWRBlueprintLibrary.generated.h"

UCLASS()
class ANIMEWORLDRUNNER_API UAWRBlueprintLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    // Mobile performance scaling
    UFUNCTION(BlueprintCallable, Category = "Performance")
    static void AdjustGraphicsForDevice();
    
    // Texture streaming control
    UFUNCTION(BlueprintCallable, Category = "Performance")
    static void SetTextureStreaming(bool Enable);
    
    // Dynamic resolution scaling
    UFUNCTION(BlueprintCallable, Category = "Performance")
    static void SetDynamicResolution(bool Enable);
    
    // Object pooling utilities
    UFUNCTION(BlueprintCallable, Category = "Performance")
    static void PreloadAssets(const TArray<FSoftObjectPath>& Assets);
    
    // Memory management
    UFUNCTION(BlueprintCallable, Category = "Performance")
    static void ClearUnusedAssets();
    
    // Frame rate control
    UFUNCTION(BlueprintCallable, Category = "Performance")
    static void SetTargetFramerate(int32 FrameRate);
    
    // Check device tier
    UFUNCTION(BlueprintPure, Category = "Performance")
    static int32 GetDevicePerformanceTier();
    
    // Screen resolution utilities
    UFUNCTION(BlueprintPure, Category = "Performance")
    static FVector2D GetScreenResolution();
    
    UFUNCTION(BlueprintCallable, Category = "Performance")
    static void SetScreenResolution(int32 Width, int32 Height);
    
    // Quality settings
    UFUNCTION(BlueprintCallable, Category = "Performance")
    static void SetGraphicsQuality(int32 QualityLevel);
    
    UFUNCTION(BlueprintCallable, Category = "Performance")
    static void SetShadowQuality(int32 QualityLevel);
    
    UFUNCTION(BlueprintCallable, Category = "Performance")
    static void SetTextureQuality(int32 QualityLevel);
    
    UFUNCTION(BlueprintCallable, Category = "Performance")
    static void SetEffectsQuality(int32 QualityLevel);
    
    // Battery optimization
    UFUNCTION(BlueprintCallable, Category = "Performance")
    static void EnableBatteryOptimization(bool Enable);
    
    // Device info
    UFUNCTION(BlueprintPure, Category = "Device")
    static FString GetDeviceModel();
    
    UFUNCTION(BlueprintPure, Category = "Device")
    static int32 GetDeviceMemoryMB();
    
    UFUNCTION(BlueprintPure, Category = "Device")
    static bool IsLowEndDevice();
    
    // Performance monitoring
    UFUNCTION(BlueprintPure, Category = "Performance")
    static float GetCurrentFPS();
    
    UFUNCTION(BlueprintPure, Category = "Performance")
    static float GetMemoryUsageMB();
};

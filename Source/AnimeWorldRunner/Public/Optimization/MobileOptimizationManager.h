#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Materials/MaterialParameterCollection.h"
#include "MobileOptimizationManager.generated.h"

UENUM(BlueprintType)
enum class EMobileQualityLevel : uint8
{
    Low         UMETA(DisplayName = "Low Quality"),
    Medium      UMETA(DisplayName = "Medium Quality"),
    High        UMETA(DisplayName = "High Quality"),
    Ultra       UMETA(DisplayName = "Ultra Quality"),
    Auto        UMETA(DisplayName = "Auto Detect")
};

UENUM(BlueprintType)
enum class EMobileTextureQuality : uint8
{
    Quarter     UMETA(DisplayName = "Quarter Resolution"),
    Half        UMETA(DisplayName = "Half Resolution"),
    Full        UMETA(DisplayName = "Full Resolution"),
    Native      UMETA(DisplayName = "Native Resolution")
};

USTRUCT(BlueprintType)
struct FMobileOptimizationSettings
{
    GENERATED_BODY()

    // Rendering Settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rendering")
    EMobileQualityLevel QualityLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rendering")
    EMobileTextureQuality TextureQuality;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rendering")
    bool bEnableDynamicResolution;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rendering")
    float TargetFrameRate;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rendering")
    float MinResolutionScale;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rendering")
    float MaxResolutionScale;

    // LOD Settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LOD")
    float LODBias;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LOD")
    float MaxDrawDistance;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LOD")
    int32 MaxInstancedMeshes;

    // Shadow Settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shadows")
    bool bEnableShadows;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shadows")
    float ShadowDistance;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shadows")
    int32 ShadowMapResolution;

    // Post Processing
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Post Processing")
    bool bEnablePostProcessing;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Post Processing")
    bool bEnableBloom;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Post Processing")
    bool bEnableAntiAliasing;

    // Performance Settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Performance")
    int32 MaxDrawCalls;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Performance")
    int32 MaxTriangles;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Performance")
    bool bEnableOcclusion;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Performance")
    bool bEnableInstancing;

    FMobileOptimizationSettings()
    {
        QualityLevel = EMobileQualityLevel::Medium;
        TextureQuality = EMobileTextureQuality::Half;
        bEnableDynamicResolution = true;
        TargetFrameRate = 60.0f;
        MinResolutionScale = 0.5f;
        MaxResolutionScale = 1.0f;
        
        LODBias = 1.0f;
        MaxDrawDistance = 3000.0f;
        MaxInstancedMeshes = 500;
        
        bEnableShadows = true;
        ShadowDistance = 1000.0f;
        ShadowMapResolution = 512;
        
        bEnablePostProcessing = true;
        bEnableBloom = false;
        bEnableAntiAliasing = false;
        
        MaxDrawCalls = 100;
        MaxTriangles = 100000;
        bEnableOcclusion = true;
        bEnableInstancing = true;
    }
};

USTRUCT(BlueprintType)
struct FPerformanceMetrics
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Metrics")
    float CurrentFPS;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Metrics")
    float AverageFPS;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Metrics")
    float FrameTime;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Metrics")
    int32 DrawCalls;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Metrics")
    int32 Triangles;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Metrics")
    float GPUTime;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Metrics")
    float CPUTime;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Metrics")
    float MemoryUsage;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Metrics")
    float CurrentResolutionScale;

    FPerformanceMetrics()
    {
        CurrentFPS = 60.0f;
        AverageFPS = 60.0f;
        FrameTime = 16.67f;
        DrawCalls = 0;
        Triangles = 0;
        GPUTime = 0.0f;
        CPUTime = 0.0f;
        MemoryUsage = 0.0f;
        CurrentResolutionScale = 1.0f;
    }
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ANIMEWORLDRUNNER_API UMobileOptimizationManager : public UActorComponent
{
    GENERATED_BODY()

public:
    UMobileOptimizationManager();

protected:
    virtual void BeginPlay() override;

public:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // Optimization Management
    UFUNCTION(BlueprintCallable, Category = "Mobile Optimization")
    void ApplyOptimizationSettings(const FMobileOptimizationSettings& Settings);

    UFUNCTION(BlueprintCallable, Category = "Mobile Optimization")
    void SetQualityLevel(EMobileQualityLevel QualityLevel);

    UFUNCTION(BlueprintCallable, Category = "Mobile Optimization")
    void AutoDetectQualityLevel();

    UFUNCTION(BlueprintCallable, Category = "Mobile Optimization")
    void OptimizeForDevice(const FString& DeviceName);

    // Dynamic Resolution
    UFUNCTION(BlueprintCallable, Category = "Mobile Optimization")
    void EnableDynamicResolution(bool bEnable);

    UFUNCTION(BlueprintCallable, Category = "Mobile Optimization")
    void SetResolutionScale(float Scale);

    UFUNCTION(BlueprintCallable, Category = "Mobile Optimization")
    void SetTargetFrameRate(float TargetFPS);

    // Texture Management
    UFUNCTION(BlueprintCallable, Category = "Mobile Optimization")
    void SetTextureQuality(EMobileTextureQuality Quality);

    UFUNCTION(BlueprintCallable, Category = "Mobile Optimization")
    void CompressTextures();

    UFUNCTION(BlueprintCallable, Category = "Mobile Optimization")
    void StreamTextures(bool bEnable);

    // LOD Management
    UFUNCTION(BlueprintCallable, Category = "Mobile Optimization")
    void SetLODBias(float Bias);

    UFUNCTION(BlueprintCallable, Category = "Mobile Optimization")
    void SetMaxDrawDistance(float Distance);

    UFUNCTION(BlueprintCallable, Category = "Mobile Optimization")
    void UpdateLODSettings();

    // Performance Monitoring
    UFUNCTION(BlueprintCallable, Category = "Mobile Optimization")
    FPerformanceMetrics GetPerformanceMetrics();

    UFUNCTION(BlueprintCallable, Category = "Mobile Optimization")
    bool IsPerformanceTargetMet();

    UFUNCTION(BlueprintCallable, Category = "Mobile Optimization")
    void StartPerformanceMonitoring();

    UFUNCTION(BlueprintCallable, Category = "Mobile Optimization")
    void StopPerformanceMonitoring();

    // Memory Management
    UFUNCTION(BlueprintCallable, Category = "Mobile Optimization")
    void OptimizeMemoryUsage();

    UFUNCTION(BlueprintCallable, Category = "Mobile Optimization")
    void GarbageCollect();

    UFUNCTION(BlueprintCallable, Category = "Mobile Optimization")
    float GetMemoryUsage();

    // Rendering Optimization
    UFUNCTION(BlueprintCallable, Category = "Mobile Optimization")
    void OptimizeRenderingSettings();

    UFUNCTION(BlueprintCallable, Category = "Mobile Optimization")
    void SetMaxDrawCalls(int32 MaxCalls);

    UFUNCTION(BlueprintCallable, Category = "Mobile Optimization")
    void EnableInstancing(bool bEnable);

    // Asset Optimization
    UFUNCTION(BlueprintCallable, Category = "Mobile Optimization")
    void OptimizeStaticMeshes();

    UFUNCTION(BlueprintCallable, Category = "Mobile Optimization")
    void OptimizeMaterials();

    UFUNCTION(BlueprintCallable, Category = "Mobile Optimization")
    void OptimizeParticleSystems();

protected:
    // Settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
    FMobileOptimizationSettings CurrentSettings;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
    TMap<FString, FMobileOptimizationSettings> DeviceProfiles;

    // Performance Monitoring
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Performance")
    FPerformanceMetrics CurrentMetrics;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Performance")
    bool bEnablePerformanceMonitoring;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Performance")
    float PerformanceUpdateInterval;

    // Dynamic Resolution
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dynamic Resolution")
    bool bDynamicResolutionEnabled;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dynamic Resolution")
    float CurrentResolutionScale;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dynamic Resolution")
    float ResolutionAdjustmentSpeed;

    // Material Parameter Collection for global optimization
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
    class UMaterialParameterCollection* OptimizationMPC;

    // Timers and tracking
    UPROPERTY()
    TArray<float> FPSHistory;

    UPROPERTY()
    float PerformanceTimer;

    UPROPERTY()
    int32 FrameCounter;

private:
    // Helper functions
    void InitializeDeviceProfiles();
    void UpdatePerformanceMetrics(float DeltaTime);
    void AdjustDynamicResolution();
    void ApplyQualitySettings(EMobileQualityLevel QualityLevel);
    FString DetectDeviceModel();
    void SetConsoleVariables();
    void OptimizeForLowEnd();
    void OptimizeForMidRange();
    void OptimizeForHighEnd();
};

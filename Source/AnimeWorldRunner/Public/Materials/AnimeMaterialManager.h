#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/Texture2D.h"
#include "AnimeMaterialManager.generated.h"

UENUM(BlueprintType)
enum class EAnimeMaterialType : uint8
{
    Character       UMETA(DisplayName = "Character Material"),
    Hair            UMETA(DisplayName = "Hair Material"),
    Clothing        UMETA(DisplayName = "Clothing Material"),
    Skin            UMETA(DisplayName = "Skin Material"),
    Environment     UMETA(DisplayName = "Environment Material"),
    Water           UMETA(DisplayName = "Water Material"),
    Foliage         UMETA(DisplayName = "Foliage Material"),
    UI              UMETA(DisplayName = "UI Material"),
    VFX             UMETA(DisplayName = "VFX Material")
};

USTRUCT(BlueprintType)
struct FAnimeMaterialSettings
{
    GENERATED_BODY()

    // Base material properties
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
    UMaterialInterface* BaseMaterial;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
    UTexture2D* BaseColorTexture;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
    UTexture2D* NormalTexture;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
    UTexture2D* PackedTexture; // R: Metallic, G: Roughness, B: AO, A: Emission

    // Cel-shading parameters
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cel Shading")
    FLinearColor MainColor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cel Shading")
    FLinearColor ShadowColor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cel Shading")
    float ShadowThreshold;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cel Shading")
    float ShadowSmoothness;

    // Rim lighting
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rim Lighting")
    FLinearColor RimColor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rim Lighting")
    float RimIntensity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rim Lighting")
    float RimPower;

    // Outline
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Outline")
    FLinearColor OutlineColor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Outline")
    float OutlineThickness;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Outline")
    bool bEnableOutline;

    // Special effects
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
    float EmissionIntensity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
    FLinearColor EmissionColor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
    float WindStrength;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
    float AnimationSpeed;

    FAnimeMaterialSettings()
    {
        BaseMaterial = nullptr;
        BaseColorTexture = nullptr;
        NormalTexture = nullptr;
        PackedTexture = nullptr;
        MainColor = FLinearColor::White;
        ShadowColor = FLinearColor(0.3f, 0.3f, 0.5f, 1.0f);
        ShadowThreshold = 0.5f;
        ShadowSmoothness = 0.1f;
        RimColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
        RimIntensity = 1.0f;
        RimPower = 2.0f;
        OutlineColor = FLinearColor::Black;
        OutlineThickness = 0.01f;
        bEnableOutline = true;
        EmissionIntensity = 0.0f;
        EmissionColor = FLinearColor::White;
        WindStrength = 0.0f;
        AnimationSpeed = 1.0f;
    }
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ANIMEWORLDRUNNER_API UAnimeMaterialManager : public UActorComponent
{
    GENERATED_BODY()

public:
    UAnimeMaterialManager();

protected:
    virtual void BeginPlay() override;

public:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // Material creation and management
    UFUNCTION(BlueprintCallable, Category = "Anime Materials")
    UMaterialInstanceDynamic* CreateAnimeMaterial(EAnimeMaterialType MaterialType, const FAnimeMaterialSettings& Settings);

    UFUNCTION(BlueprintCallable, Category = "Anime Materials")
    void ApplyMaterialToMesh(UMeshComponent* MeshComponent, UMaterialInstanceDynamic* Material, int32 MaterialIndex = 0);

    UFUNCTION(BlueprintCallable, Category = "Anime Materials")
    void UpdateMaterialParameters(UMaterialInstanceDynamic* Material, const FAnimeMaterialSettings& Settings);

    // Preset material configurations
    UFUNCTION(BlueprintCallable, Category = "Anime Materials")
    UMaterialInstanceDynamic* CreateCharacterMaterial(FLinearColor MainColor, FLinearColor ShadowColor);

    UFUNCTION(BlueprintCallable, Category = "Anime Materials")
    UMaterialInstanceDynamic* CreateHairMaterial(FLinearColor HairColor, float Shininess);

    UFUNCTION(BlueprintCallable, Category = "Anime Materials")
    UMaterialInstanceDynamic* CreateClothingMaterial(FLinearColor ClothColor, UTexture2D* Pattern = nullptr);

    UFUNCTION(BlueprintCallable, Category = "Anime Materials")
    UMaterialInstanceDynamic* CreateEnvironmentMaterial(UTexture2D* BaseTexture, float TileScale = 1.0f);

    UFUNCTION(BlueprintCallable, Category = "Anime Materials")
    UMaterialInstanceDynamic* CreateWaterMaterial(FLinearColor WaterColor, float WaveSpeed = 1.0f);

    // Material animation and effects
    UFUNCTION(BlueprintCallable, Category = "Anime Materials")
    void StartMaterialAnimation(UMaterialInstanceDynamic* Material, FName ParameterName, float StartValue, float EndValue, float Duration);

    UFUNCTION(BlueprintCallable, Category = "Anime Materials")
    void SetGlobalLightDirection(FVector LightDirection);

    UFUNCTION(BlueprintCallable, Category = "Anime Materials")
    void SetGlobalTimeOfDay(float TimeOfDay);

    // Mobile optimization
    UFUNCTION(BlueprintCallable, Category = "Anime Materials")
    void OptimizeForMobile(bool bEnableOptimization);

    UFUNCTION(BlueprintCallable, Category = "Anime Materials")
    void SetQualityLevel(int32 QualityLevel);

protected:
    // Material templates
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material Templates")
    TMap<EAnimeMaterialType, UMaterialInterface*> MaterialTemplates;

    // Global material parameters
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Global Settings")
    FVector GlobalLightDirection;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Global Settings")
    float GlobalTimeOfDay;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Global Settings")
    bool bMobileOptimization;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Global Settings")
    int32 CurrentQualityLevel;

    // Active materials for global updates
    UPROPERTY()
    TArray<UMaterialInstanceDynamic*> ActiveMaterials;

    // Animation system
    UPROPERTY()
    TMap<UMaterialInstanceDynamic*, FTimerHandle> MaterialAnimationTimers;

private:
    void InitializeMaterialTemplates();
    void UpdateGlobalParameters();
    void UpdateMaterialAnimation(UMaterialInstanceDynamic* Material, FName ParameterName, float StartValue, float EndValue, float Duration, float ElapsedTime);
};

#include "Materials/AnimeMaterialManager.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "Components/MeshComponent.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"

UAnimeMaterialManager::UAnimeMaterialManager()
{
    PrimaryComponentTick.bCanEverTick = true;
    
    // Initialize global settings
    GlobalLightDirection = FVector(0.5f, 0.5f, -1.0f).GetSafeNormal();
    GlobalTimeOfDay = 0.5f; // Noon
    bMobileOptimization = true;
    CurrentQualityLevel = 2; // Medium quality by default
}

void UAnimeMaterialManager::BeginPlay()
{
    Super::BeginPlay();
    
    InitializeMaterialTemplates();
}

void UAnimeMaterialManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    
    UpdateGlobalParameters();
}

void UAnimeMaterialManager::InitializeMaterialTemplates()
{
    // Initialize default material templates
    // These would be set in Blueprint or loaded from assets
    
    // For now, we'll leave them empty and they can be assigned in Blueprint
    MaterialTemplates.Empty();
}

UMaterialInstanceDynamic* UAnimeMaterialManager::CreateAnimeMaterial(EAnimeMaterialType MaterialType, const FAnimeMaterialSettings& Settings)
{
    UMaterialInterface* BaseMaterial = nullptr;
    
    // Get base material template
    if (UMaterialInterface** FoundTemplate = MaterialTemplates.Find(MaterialType))
    {
        BaseMaterial = *FoundTemplate;
    }
    else if (Settings.BaseMaterial)
    {
        BaseMaterial = Settings.BaseMaterial;
    }
    
    if (!BaseMaterial)
    {
        UE_LOG(LogTemp, Warning, TEXT("No base material found for material type"));
        return nullptr;
    }
    
    // Create dynamic material instance
    UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, this);
    
    if (DynamicMaterial)
    {
        UpdateMaterialParameters(DynamicMaterial, Settings);
        ActiveMaterials.Add(DynamicMaterial);
    }
    
    return DynamicMaterial;
}

void UAnimeMaterialManager::ApplyMaterialToMesh(UMeshComponent* MeshComponent, UMaterialInstanceDynamic* Material, int32 MaterialIndex)
{
    if (MeshComponent && Material)
    {
        MeshComponent->SetMaterial(MaterialIndex, Material);
    }
}

void UAnimeMaterialManager::UpdateMaterialParameters(UMaterialInstanceDynamic* Material, const FAnimeMaterialSettings& Settings)
{
    if (!Material) return;
    
    // Set base textures
    if (Settings.BaseColorTexture)
    {
        Material->SetTextureParameterValue(FName("BaseColorTexture"), Settings.BaseColorTexture);
    }
    
    if (Settings.NormalTexture)
    {
        Material->SetTextureParameterValue(FName("NormalTexture"), Settings.NormalTexture);
    }
    
    if (Settings.PackedTexture)
    {
        Material->SetTextureParameterValue(FName("PackedTexture"), Settings.PackedTexture);
    }
    
    // Set cel-shading parameters
    Material->SetVectorParameterValue(FName("MainColor"), Settings.MainColor);
    Material->SetVectorParameterValue(FName("ShadowColor"), Settings.ShadowColor);
    Material->SetScalarParameterValue(FName("ShadowThreshold"), Settings.ShadowThreshold);
    Material->SetScalarParameterValue(FName("ShadowSmoothness"), Settings.ShadowSmoothness);
    
    // Set rim lighting parameters
    Material->SetVectorParameterValue(FName("RimColor"), Settings.RimColor);
    Material->SetScalarParameterValue(FName("RimIntensity"), Settings.RimIntensity);
    Material->SetScalarParameterValue(FName("RimPower"), Settings.RimPower);
    
    // Set outline parameters
    Material->SetVectorParameterValue(FName("OutlineColor"), Settings.OutlineColor);
    Material->SetScalarParameterValue(FName("OutlineThickness"), Settings.OutlineThickness);
    Material->SetScalarParameterValue(FName("EnableOutline"), Settings.bEnableOutline ? 1.0f : 0.0f);
    
    // Set special effects
    Material->SetScalarParameterValue(FName("EmissionIntensity"), Settings.EmissionIntensity);
    Material->SetVectorParameterValue(FName("EmissionColor"), Settings.EmissionColor);
    Material->SetScalarParameterValue(FName("WindStrength"), Settings.WindStrength);
    Material->SetScalarParameterValue(FName("AnimationSpeed"), Settings.AnimationSpeed);
    
    // Set global parameters
    Material->SetVectorParameterValue(FName("GlobalLightDirection"), FLinearColor(GlobalLightDirection.X, GlobalLightDirection.Y, GlobalLightDirection.Z, 0.0f));
    Material->SetScalarParameterValue(FName("TimeOfDay"), GlobalTimeOfDay);
}

UMaterialInstanceDynamic* UAnimeMaterialManager::CreateCharacterMaterial(FLinearColor MainColor, FLinearColor ShadowColor)
{
    FAnimeMaterialSettings Settings;
    Settings.MainColor = MainColor;
    Settings.ShadowColor = ShadowColor;
    Settings.ShadowThreshold = 0.5f;
    Settings.ShadowSmoothness = 0.1f;
    Settings.RimColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
    Settings.RimIntensity = 0.8f;
    Settings.RimPower = 3.0f;
    Settings.bEnableOutline = true;
    Settings.OutlineThickness = 0.008f;
    
    return CreateAnimeMaterial(EAnimeMaterialType::Character, Settings);
}

UMaterialInstanceDynamic* UAnimeMaterialManager::CreateHairMaterial(FLinearColor HairColor, float Shininess)
{
    FAnimeMaterialSettings Settings;
    Settings.MainColor = HairColor;
    Settings.ShadowColor = HairColor * 0.6f;
    Settings.ShadowThreshold = 0.4f;
    Settings.ShadowSmoothness = 0.2f;
    Settings.RimColor = FLinearColor::White;
    Settings.RimIntensity = Shininess;
    Settings.RimPower = 4.0f;
    Settings.bEnableOutline = true;
    Settings.OutlineThickness = 0.006f;
    
    return CreateAnimeMaterial(EAnimeMaterialType::Hair, Settings);
}

UMaterialInstanceDynamic* UAnimeMaterialManager::CreateClothingMaterial(FLinearColor ClothColor, UTexture2D* Pattern)
{
    FAnimeMaterialSettings Settings;
    Settings.MainColor = ClothColor;
    Settings.ShadowColor = ClothColor * 0.7f;
    Settings.ShadowThreshold = 0.6f;
    Settings.ShadowSmoothness = 0.15f;
    Settings.RimColor = FLinearColor(0.9f, 0.9f, 1.0f, 1.0f);
    Settings.RimIntensity = 0.5f;
    Settings.RimPower = 2.5f;
    Settings.bEnableOutline = true;
    Settings.OutlineThickness = 0.01f;
    Settings.BaseColorTexture = Pattern;
    
    return CreateAnimeMaterial(EAnimeMaterialType::Clothing, Settings);
}

UMaterialInstanceDynamic* UAnimeMaterialManager::CreateEnvironmentMaterial(UTexture2D* BaseTexture, float TileScale)
{
    FAnimeMaterialSettings Settings;
    Settings.MainColor = FLinearColor::White;
    Settings.ShadowColor = FLinearColor(0.4f, 0.4f, 0.6f, 1.0f);
    Settings.ShadowThreshold = 0.7f;
    Settings.ShadowSmoothness = 0.2f;
    Settings.RimColor = FLinearColor::White;
    Settings.RimIntensity = 0.3f;
    Settings.RimPower = 1.5f;
    Settings.bEnableOutline = false;
    Settings.BaseColorTexture = BaseTexture;
    
    UMaterialInstanceDynamic* Material = CreateAnimeMaterial(EAnimeMaterialType::Environment, Settings);
    
    if (Material)
    {
        Material->SetScalarParameterValue(FName("TileScale"), TileScale);
    }
    
    return Material;
}

UMaterialInstanceDynamic* UAnimeMaterialManager::CreateWaterMaterial(FLinearColor WaterColor, float WaveSpeed)
{
    FAnimeMaterialSettings Settings;
    Settings.MainColor = WaterColor;
    Settings.ShadowColor = WaterColor * 0.8f;
    Settings.ShadowThreshold = 0.3f;
    Settings.ShadowSmoothness = 0.3f;
    Settings.RimColor = FLinearColor::White;
    Settings.RimIntensity = 1.2f;
    Settings.RimPower = 1.0f;
    Settings.bEnableOutline = false;
    Settings.AnimationSpeed = WaveSpeed;
    Settings.EmissionIntensity = 0.2f;
    Settings.EmissionColor = WaterColor;
    
    UMaterialInstanceDynamic* Material = CreateAnimeMaterial(EAnimeMaterialType::Water, Settings);
    
    if (Material)
    {
        Material->SetScalarParameterValue(FName("WaveSpeed"), WaveSpeed);
        Material->SetScalarParameterValue(FName("WaveScale"), 1.0f);
        Material->SetScalarParameterValue(FName("Refraction"), 0.1f);
    }
    
    return Material;
}

void UAnimeMaterialManager::StartMaterialAnimation(UMaterialInstanceDynamic* Material, FName ParameterName, float StartValue, float EndValue, float Duration)
{
    if (!Material || Duration <= 0.0f) return;
    
    // Set initial value
    Material->SetScalarParameterValue(ParameterName, StartValue);
    
    // Create timer for animation
    FTimerHandle AnimationTimer;
    float ElapsedTime = 0.0f;
    
    GetWorld()->GetTimerManager().SetTimer(AnimationTimer, [this, Material, ParameterName, StartValue, EndValue, Duration, ElapsedTime]() mutable
    {
        ElapsedTime += 0.016f; // ~60 FPS
        float Alpha = FMath::Clamp(ElapsedTime / Duration, 0.0f, 1.0f);
        float CurrentValue = FMath::Lerp(StartValue, EndValue, Alpha);
        
        if (Material)
        {
            Material->SetScalarParameterValue(ParameterName, CurrentValue);
        }
        
        if (Alpha >= 1.0f)
        {
            GetWorld()->GetTimerManager().ClearTimer(MaterialAnimationTimers[Material]);
            MaterialAnimationTimers.Remove(Material);
        }
    }, 0.016f, true);
    
    MaterialAnimationTimers.Add(Material, AnimationTimer);
}

void UAnimeMaterialManager::SetGlobalLightDirection(FVector LightDirection)
{
    GlobalLightDirection = LightDirection.GetSafeNormal();
}

void UAnimeMaterialManager::SetGlobalTimeOfDay(float TimeOfDay)
{
    GlobalTimeOfDay = FMath::Clamp(TimeOfDay, 0.0f, 1.0f);
}

void UAnimeMaterialManager::OptimizeForMobile(bool bEnableOptimization)
{
    bMobileOptimization = bEnableOptimization;
    
    if (bMobileOptimization)
    {
        // Apply mobile optimizations to all active materials
        for (UMaterialInstanceDynamic* Material : ActiveMaterials)
        {
            if (Material)
            {
                // Reduce outline thickness for mobile
                float CurrentOutlineThickness;
                if (Material->GetScalarParameterValue(FName("OutlineThickness"), CurrentOutlineThickness))
                {
                    Material->SetScalarParameterValue(FName("OutlineThickness"), CurrentOutlineThickness * 0.7f);
                }
                
                // Reduce rim lighting intensity
                float CurrentRimIntensity;
                if (Material->GetScalarParameterValue(FName("RimIntensity"), CurrentRimIntensity))
                {
                    Material->SetScalarParameterValue(FName("RimIntensity"), CurrentRimIntensity * 0.8f);
                }
                
                // Increase shadow smoothness for better performance
                Material->SetScalarParameterValue(FName("ShadowSmoothness"), 0.2f);
            }
        }
    }
}

void UAnimeMaterialManager::SetQualityLevel(int32 QualityLevel)
{
    CurrentQualityLevel = FMath::Clamp(QualityLevel, 0, 3);
    
    float QualityMultiplier = 1.0f;
    
    switch (CurrentQualityLevel)
    {
        case 0: // Low
            QualityMultiplier = 0.5f;
            break;
        case 1: // Medium
            QualityMultiplier = 0.75f;
            break;
        case 2: // High
            QualityMultiplier = 1.0f;
            break;
        case 3: // Ultra
            QualityMultiplier = 1.25f;
            break;
    }
    
    // Apply quality settings to all active materials
    for (UMaterialInstanceDynamic* Material : ActiveMaterials)
    {
        if (Material)
        {
            // Adjust outline thickness based on quality
            float BaseOutlineThickness;
            if (Material->GetScalarParameterValue(FName("OutlineThickness"), BaseOutlineThickness))
            {
                Material->SetScalarParameterValue(FName("OutlineThickness"), BaseOutlineThickness * QualityMultiplier);
            }
            
            // Adjust rim lighting based on quality
            float BaseRimIntensity;
            if (Material->GetScalarParameterValue(FName("RimIntensity"), BaseRimIntensity))
            {
                Material->SetScalarParameterValue(FName("RimIntensity"), BaseRimIntensity * QualityMultiplier);
            }
        }
    }
}

void UAnimeMaterialManager::UpdateGlobalParameters()
{
    // Update time-based parameters for all active materials
    float CurrentTime = GetWorld()->GetTimeSeconds();
    
    for (UMaterialInstanceDynamic* Material : ActiveMaterials)
    {
        if (Material)
        {
            Material->SetScalarParameterValue(FName("GlobalTime"), CurrentTime);
            Material->SetVectorParameterValue(FName("GlobalLightDirection"), FLinearColor(GlobalLightDirection.X, GlobalLightDirection.Y, GlobalLightDirection.Z, 0.0f));
            Material->SetScalarParameterValue(FName("TimeOfDay"), GlobalTimeOfDay);
        }
    }
}

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"
#include "Engine/World.h"
#include "AnimeEffectsManager.generated.h"

UENUM(BlueprintType)
enum class EAnimeEffectType : uint8
{
    None            UMETA(DisplayName = "None"),
    Dash            UMETA(DisplayName = "Dash Effect"),
    Jump            UMETA(DisplayName = "Jump Effect"),
    Landing         UMETA(DisplayName = "Landing Effect"),
    Running         UMETA(DisplayName = "Running Effect"),
    Attack          UMETA(DisplayName = "Attack Effect"),
    SpellCast       UMETA(DisplayName = "Spell Cast Effect"),
    PowerUp         UMETA(DisplayName = "Power Up Effect"),
    Collect         UMETA(DisplayName = "Collect Effect"),
    Glide           UMETA(DisplayName = "Glide Effect"),
    Climb           UMETA(DisplayName = "Climb Effect"),
    Hit             UMETA(DisplayName = "Hit Effect"),
    Explosion       UMETA(DisplayName = "Explosion Effect"),
    Heal            UMETA(DisplayName = "Heal Effect"),
    Shield          UMETA(DisplayName = "Shield Effect")
};

USTRUCT(BlueprintType)
struct FAnimeEffectData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
    UParticleSystem* ParticleEffect;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
    USoundCue* SoundEffect;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
    float Duration;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
    FVector Scale;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
    FLinearColor Color;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
    bool bAttachToActor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
    FName AttachSocketName;

    FAnimeEffectData()
    {
        ParticleEffect = nullptr;
        SoundEffect = nullptr;
        Duration = 1.0f;
        Scale = FVector(1.0f, 1.0f, 1.0f);
        Color = FLinearColor::White;
        bAttachToActor = true;
        AttachSocketName = NAME_None;
    }
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ANIMEWORLDRUNNER_API UAnimeEffectsManager : public UActorComponent
{
    GENERATED_BODY()

public:
    UAnimeEffectsManager();

protected:
    virtual void BeginPlay() override;

public:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // Main effect functions
    UFUNCTION(BlueprintCallable, Category = "Anime Effects")
    void PlayEffect(EAnimeEffectType EffectType, FVector Location = FVector::ZeroVector, FRotator Rotation = FRotator::ZeroRotator);

    UFUNCTION(BlueprintCallable, Category = "Anime Effects")
    void PlayEffectAtLocation(EAnimeEffectType EffectType, FVector Location, FRotator Rotation = FRotator::ZeroRotator);

    UFUNCTION(BlueprintCallable, Category = "Anime Effects")
    void PlayEffectAttached(EAnimeEffectType EffectType, USceneComponent* AttachComponent, FName SocketName = NAME_None);

    UFUNCTION(BlueprintCallable, Category = "Anime Effects")
    void StopEffect(EAnimeEffectType EffectType);

    UFUNCTION(BlueprintCallable, Category = "Anime Effects")
    void StopAllEffects();

    // Specialized anime-style effects
    UFUNCTION(BlueprintCallable, Category = "Anime Effects")
    void PlayDashTrail(FVector StartLocation, FVector EndLocation);

    UFUNCTION(BlueprintCallable, Category = "Anime Effects")
    void PlayElementalBurst(FVector Location, FLinearColor ElementColor);

    UFUNCTION(BlueprintCallable, Category = "Anime Effects")
    void PlayImpactEffect(FVector Location, FVector Normal);

    UFUNCTION(BlueprintCallable, Category = "Anime Effects")
    void PlayAuraEffect(bool bEnable, FLinearColor AuraColor = FLinearColor::White);

    // Screen effects
    UFUNCTION(BlueprintCallable, Category = "Anime Effects")
    void PlayScreenFlash(FLinearColor FlashColor, float Duration = 0.2f);

    UFUNCTION(BlueprintCallable, Category = "Anime Effects")
    void PlayCameraShake(float Intensity = 1.0f, float Duration = 0.5f);

protected:
    // Effect data mapping
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect Data")
    TMap<EAnimeEffectType, FAnimeEffectData> EffectDataMap;

    // Active effect components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Active Effects")
    TMap<EAnimeEffectType, UParticleSystemComponent*> ActiveParticleEffects;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Active Effects")
    TMap<EAnimeEffectType, UAudioComponent*> ActiveAudioEffects;

    // Persistent effects
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Persistent Effects")
    UParticleSystemComponent* AuraEffectComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Persistent Effects")
    UParticleSystemComponent* TrailEffectComponent;

    // Effect settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
    float GlobalEffectScale;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
    float GlobalVolumeMultiplier;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
    bool bEnableScreenEffects;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
    bool bEnableCameraShake;

private:
    // Helper functions
    void InitializeEffectData();
    void CleanupExpiredEffects();
    UParticleSystemComponent* CreateParticleComponent(UParticleSystem* ParticleSystem);
    UAudioComponent* CreateAudioComponent(USoundCue* SoundCue);

    // Timer handles for cleanup
    FTimerHandle EffectCleanupTimer;
    
    // Screen effect materials (to be set in Blueprint)
    UPROPERTY(EditAnywhere, Category = "Screen Effects")
    class UMaterialInterface* ScreenFlashMaterial;
};

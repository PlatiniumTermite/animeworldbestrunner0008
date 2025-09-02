#include "Effects/AnimeEffectsManager.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Components/SceneComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/AudioComponent.h"
#include "Camera/CameraShakeBase.h"
#include "Engine/Engine.h"

UAnimeEffectsManager::UAnimeEffectsManager()
{
    PrimaryComponentTick.bCanEverTick = true;
    
    // Initialize settings
    GlobalEffectScale = 1.0f;
    GlobalVolumeMultiplier = 1.0f;
    bEnableScreenEffects = true;
    bEnableCameraShake = true;
    
    // Create persistent effect components
    AuraEffectComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("AuraEffect"));
    AuraEffectComponent->bAutoActivate = false;
    
    TrailEffectComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("TrailEffect"));
    TrailEffectComponent->bAutoActivate = false;
}

void UAnimeEffectsManager::BeginPlay()
{
    Super::BeginPlay();
    
    // Initialize effect data
    InitializeEffectData();
    
    // Setup persistent effects
    if (AuraEffectComponent)
    {
        AuraEffectComponent->AttachToComponent(GetOwner()->GetRootComponent(), 
            FAttachmentTransformRules::KeepRelativeTransform);
    }
    
    if (TrailEffectComponent)
    {
        TrailEffectComponent->AttachToComponent(GetOwner()->GetRootComponent(), 
            FAttachmentTransformRules::KeepRelativeTransform);
    }
    
    // Setup cleanup timer
    GetWorld()->GetTimerManager().SetTimer(EffectCleanupTimer, this, 
        &UAnimeEffectsManager::CleanupExpiredEffects, 1.0f, true);
}

void UAnimeEffectsManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    
    // Update dynamic effects here if needed
}

void UAnimeEffectsManager::InitializeEffectData()
{
    // Initialize default effect data for each type
    // These can be overridden in Blueprint or through asset assignment
    
    FAnimeEffectData DashEffect;
    DashEffect.Duration = 0.5f;
    DashEffect.Scale = FVector(1.5f, 1.5f, 1.0f);
    DashEffect.Color = FLinearColor(0.0f, 0.8f, 1.0f, 1.0f); // Cyan
    DashEffect.bAttachToActor = true;
    EffectDataMap.Add(EAnimeEffectType::Dash, DashEffect);
    
    FAnimeEffectData JumpEffect;
    JumpEffect.Duration = 0.3f;
    JumpEffect.Scale = FVector(1.0f, 1.0f, 1.0f);
    JumpEffect.Color = FLinearColor(1.0f, 1.0f, 0.0f, 1.0f); // Yellow
    JumpEffect.bAttachToActor = false;
    EffectDataMap.Add(EAnimeEffectType::Jump, JumpEffect);
    
    FAnimeEffectData LandingEffect;
    LandingEffect.Duration = 0.4f;
    LandingEffect.Scale = FVector(1.2f, 1.2f, 0.5f);
    LandingEffect.Color = FLinearColor(0.8f, 0.6f, 0.4f, 1.0f); // Dust color
    LandingEffect.bAttachToActor = false;
    EffectDataMap.Add(EAnimeEffectType::Landing, LandingEffect);
    
    FAnimeEffectData RunningEffect;
    RunningEffect.Duration = -1.0f; // Continuous
    RunningEffect.Scale = FVector(0.8f, 0.8f, 0.8f);
    RunningEffect.Color = FLinearColor(0.9f, 0.9f, 0.9f, 0.5f);
    RunningEffect.bAttachToActor = true;
    RunningEffect.AttachSocketName = FName("foot_l"); // Left foot socket
    EffectDataMap.Add(EAnimeEffectType::Running, RunningEffect);
    
    FAnimeEffectData AttackEffect;
    AttackEffect.Duration = 0.6f;
    AttackEffect.Scale = FVector(1.3f, 1.3f, 1.3f);
    AttackEffect.Color = FLinearColor(1.0f, 0.3f, 0.0f, 1.0f); // Orange-red
    AttackEffect.bAttachToActor = true;
    AttackEffect.AttachSocketName = FName("hand_r"); // Right hand socket
    EffectDataMap.Add(EAnimeEffectType::Attack, AttackEffect);
    
    FAnimeEffectData SpellCastEffect;
    SpellCastEffect.Duration = 1.2f;
    SpellCastEffect.Scale = FVector(1.5f, 1.5f, 1.5f);
    SpellCastEffect.Color = FLinearColor(0.5f, 0.0f, 1.0f, 1.0f); // Purple
    SpellCastEffect.bAttachToActor = true;
    SpellCastEffect.AttachSocketName = FName("hand_l"); // Left hand socket
    EffectDataMap.Add(EAnimeEffectType::SpellCast, SpellCastEffect);
    
    FAnimeEffectData PowerUpEffect;
    PowerUpEffect.Duration = 2.0f;
    PowerUpEffect.Scale = FVector(2.0f, 2.0f, 2.0f);
    PowerUpEffect.Color = FLinearColor(1.0f, 1.0f, 0.0f, 1.0f); // Gold
    PowerUpEffect.bAttachToActor = true;
    EffectDataMap.Add(EAnimeEffectType::PowerUp, PowerUpEffect);
    
    FAnimeEffectData CollectEffect;
    CollectEffect.Duration = 0.8f;
    CollectEffect.Scale = FVector(1.0f, 1.0f, 1.0f);
    CollectEffect.Color = FLinearColor(0.0f, 1.0f, 0.0f, 1.0f); // Green
    CollectEffect.bAttachToActor = false;
    EffectDataMap.Add(EAnimeEffectType::Collect, CollectEffect);
    
    FAnimeEffectData GlideEffect;
    GlideEffect.Duration = -1.0f; // Continuous
    GlideEffect.Scale = FVector(1.8f, 1.8f, 1.0f);
    GlideEffect.Color = FLinearColor(0.7f, 0.9f, 1.0f, 0.8f); // Light blue
    GlideEffect.bAttachToActor = true;
    EffectDataMap.Add(EAnimeEffectType::Glide, GlideEffect);
    
    FAnimeEffectData ClimbEffect;
    ClimbEffect.Duration = -1.0f; // Continuous
    ClimbEffect.Scale = FVector(0.6f, 0.6f, 0.6f);
    ClimbEffect.Color = FLinearColor(0.8f, 0.8f, 0.6f, 0.7f); // Dusty yellow
    ClimbEffect.bAttachToActor = true;
    EffectDataMap.Add(EAnimeEffectType::Climb, ClimbEffect);
}

void UAnimeEffectsManager::PlayEffect(EAnimeEffectType EffectType, FVector Location, FRotator Rotation)
{
    if (EffectType == EAnimeEffectType::None) return;
    
    FAnimeEffectData* EffectData = EffectDataMap.Find(EffectType);
    if (!EffectData) return;
    
    // Stop existing effect of same type
    StopEffect(EffectType);
    
    // Play particle effect
    if (EffectData->ParticleEffect)
    {
        UParticleSystemComponent* ParticleComp = CreateParticleComponent(EffectData->ParticleEffect);
        if (ParticleComp)
        {
            ParticleComp->SetWorldLocationAndRotation(Location, Rotation);
            ParticleComp->SetWorldScale3D(EffectData->Scale * GlobalEffectScale);
            
            // Set color parameter if supported
            ParticleComp->SetColorParameter(FName("Color"), EffectData->Color);
            
            ParticleComp->Activate();
            ActiveParticleEffects.Add(EffectType, ParticleComp);
            
            // Auto-destroy after duration (if not continuous)
            if (EffectData->Duration > 0.0f)
            {
                FTimerHandle DestroyTimer;
                GetWorld()->GetTimerManager().SetTimer(DestroyTimer, [this, EffectType]()
                {
                    StopEffect(EffectType);
                }, EffectData->Duration, false);
            }
        }
    }
    
    // Play sound effect
    if (EffectData->SoundEffect)
    {
        UAudioComponent* AudioComp = CreateAudioComponent(EffectData->SoundEffect);
        if (AudioComp)
        {
            AudioComp->SetWorldLocation(Location);
            AudioComp->SetVolumeMultiplier(GlobalVolumeMultiplier);
            AudioComp->Play();
            ActiveAudioEffects.Add(EffectType, AudioComp);
        }
    }
}

void UAnimeEffectsManager::PlayEffectAtLocation(EAnimeEffectType EffectType, FVector Location, FRotator Rotation)
{
    PlayEffect(EffectType, Location, Rotation);
}

void UAnimeEffectsManager::PlayEffectAttached(EAnimeEffectType EffectType, USceneComponent* AttachComponent, FName SocketName)
{
    if (EffectType == EAnimeEffectType::None || !AttachComponent) return;
    
    FAnimeEffectData* EffectData = EffectDataMap.Find(EffectType);
    if (!EffectData) return;
    
    // Stop existing effect of same type
    StopEffect(EffectType);
    
    // Play particle effect
    if (EffectData->ParticleEffect)
    {
        UParticleSystemComponent* ParticleComp = CreateParticleComponent(EffectData->ParticleEffect);
        if (ParticleComp)
        {
            FName AttachSocket = (SocketName != NAME_None) ? SocketName : EffectData->AttachSocketName;
            ParticleComp->AttachToComponent(AttachComponent, 
                FAttachmentTransformRules::KeepRelativeTransform, AttachSocket);
            
            ParticleComp->SetRelativeScale3D(EffectData->Scale * GlobalEffectScale);
            ParticleComp->SetColorParameter(FName("Color"), EffectData->Color);
            ParticleComp->Activate();
            
            ActiveParticleEffects.Add(EffectType, ParticleComp);
            
            // Auto-destroy after duration (if not continuous)
            if (EffectData->Duration > 0.0f)
            {
                FTimerHandle DestroyTimer;
                GetWorld()->GetTimerManager().SetTimer(DestroyTimer, [this, EffectType]()
                {
                    StopEffect(EffectType);
                }, EffectData->Duration, false);
            }
        }
    }
}

void UAnimeEffectsManager::StopEffect(EAnimeEffectType EffectType)
{
    // Stop particle effect
    if (UParticleSystemComponent** ParticleComp = ActiveParticleEffects.Find(EffectType))
    {
        if (*ParticleComp && IsValid(*ParticleComp))
        {
            (*ParticleComp)->Deactivate();
            (*ParticleComp)->DestroyComponent();
        }
        ActiveParticleEffects.Remove(EffectType);
    }
    
    // Stop audio effect
    if (UAudioComponent** AudioComp = ActiveAudioEffects.Find(EffectType))
    {
        if (*AudioComp && IsValid(*AudioComp))
        {
            (*AudioComp)->Stop();
            (*AudioComp)->DestroyComponent();
        }
        ActiveAudioEffects.Remove(EffectType);
    }
}

void UAnimeEffectsManager::StopAllEffects()
{
    // Stop all particle effects
    for (auto& EffectPair : ActiveParticleEffects)
    {
        if (EffectPair.Value && IsValid(EffectPair.Value))
        {
            EffectPair.Value->Deactivate();
            EffectPair.Value->DestroyComponent();
        }
    }
    ActiveParticleEffects.Empty();
    
    // Stop all audio effects
    for (auto& EffectPair : ActiveAudioEffects)
    {
        if (EffectPair.Value && IsValid(EffectPair.Value))
        {
            EffectPair.Value->Stop();
            EffectPair.Value->DestroyComponent();
        }
    }
    ActiveAudioEffects.Empty();
}

void UAnimeEffectsManager::PlayDashTrail(FVector StartLocation, FVector EndLocation)
{
    if (TrailEffectComponent)
    {
        TrailEffectComponent->SetWorldLocation(StartLocation);
        
        // Set trail parameters
        TrailEffectComponent->SetVectorParameter(FName("EndLocation"), EndLocation);
        TrailEffectComponent->SetColorParameter(FName("TrailColor"), FLinearColor(0.0f, 0.8f, 1.0f, 1.0f));
        
        TrailEffectComponent->Activate();
        
        // Deactivate after short duration
        FTimerHandle TrailTimer;
        GetWorld()->GetTimerManager().SetTimer(TrailTimer, [this]()
        {
            if (TrailEffectComponent)
            {
                TrailEffectComponent->Deactivate();
            }
        }, 1.0f, false);
    }
}

void UAnimeEffectsManager::PlayElementalBurst(FVector Location, FLinearColor ElementColor)
{
    // Create a large burst effect at location
    FAnimeEffectData BurstData;
    BurstData.Duration = 2.0f;
    BurstData.Scale = FVector(3.0f, 3.0f, 3.0f);
    BurstData.Color = ElementColor;
    BurstData.bAttachToActor = false;
    
    // Temporarily add to effect map
    EffectDataMap.Add(EAnimeEffectType::Explosion, BurstData);
    PlayEffectAtLocation(EAnimeEffectType::Explosion, Location);
    
    // Play camera shake
    if (bEnableCameraShake)
    {
        PlayCameraShake(2.0f, 1.0f);
    }
    
    // Play screen flash
    if (bEnableScreenEffects)
    {
        PlayScreenFlash(ElementColor, 0.3f);
    }
}

void UAnimeEffectsManager::PlayImpactEffect(FVector Location, FVector Normal)
{
    FRotator ImpactRotation = Normal.Rotation();
    PlayEffectAtLocation(EAnimeEffectType::Hit, Location, ImpactRotation);
    
    // Small camera shake for impact
    if (bEnableCameraShake)
    {
        PlayCameraShake(0.5f, 0.2f);
    }
}

void UAnimeEffectsManager::PlayAuraEffect(bool bEnable, FLinearColor AuraColor)
{
    if (AuraEffectComponent)
    {
        if (bEnable)
        {
            AuraEffectComponent->SetColorParameter(FName("AuraColor"), AuraColor);
            AuraEffectComponent->Activate();
        }
        else
        {
            AuraEffectComponent->Deactivate();
        }
    }
}

void UAnimeEffectsManager::PlayScreenFlash(FLinearColor FlashColor, float Duration)
{
    if (!bEnableScreenEffects) return;
    
    // This would typically use a post-process material or UI widget
    // For now, we'll use the engine's built-in screen fade
    if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
    {
        PC->ClientSetCameraFade(true, FlashColor, FVector2D(1.0f, 0.0f), Duration, false, true);
    }
}

void UAnimeEffectsManager::PlayCameraShake(float Intensity, float Duration)
{
    if (!bEnableCameraShake) return;
    
    // This would use a custom camera shake class in a full implementation
    // For now, we'll create a simple shake effect
    if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
    {
        // Use built-in camera shake if available
        // PC->ClientStartCameraShake(CameraShakeClass, Intensity);
    }
}

void UAnimeEffectsManager::CleanupExpiredEffects()
{
    // Remove any invalid components
    TArray<EAnimeEffectType> EffectsToRemove;
    
    for (auto& EffectPair : ActiveParticleEffects)
    {
        if (!IsValid(EffectPair.Value) || !EffectPair.Value->IsActive())
        {
            EffectsToRemove.Add(EffectPair.Key);
        }
    }
    
    for (EAnimeEffectType EffectType : EffectsToRemove)
    {
        ActiveParticleEffects.Remove(EffectType);
    }
    
    EffectsToRemove.Empty();
    
    for (auto& EffectPair : ActiveAudioEffects)
    {
        if (!IsValid(EffectPair.Value) || !EffectPair.Value->IsPlaying())
        {
            EffectsToRemove.Add(EffectPair.Key);
        }
    }
    
    for (EAnimeEffectType EffectType : EffectsToRemove)
    {
        ActiveAudioEffects.Remove(EffectType);
    }
}

UParticleSystemComponent* UAnimeEffectsManager::CreateParticleComponent(UParticleSystem* ParticleSystem)
{
    if (!ParticleSystem) return nullptr;
    
    UParticleSystemComponent* ParticleComp = NewObject<UParticleSystemComponent>(GetOwner());
    ParticleComp->SetTemplate(ParticleSystem);
    ParticleComp->bAutoActivate = false;
    ParticleComp->AttachToComponent(GetOwner()->GetRootComponent(), 
        FAttachmentTransformRules::KeepWorldTransform);
    
    return ParticleComp;
}

UAudioComponent* UAnimeEffectsManager::CreateAudioComponent(USoundCue* SoundCue)
{
    if (!SoundCue) return nullptr;
    
    UAudioComponent* AudioComp = NewObject<UAudioComponent>(GetOwner());
    AudioComp->SetSound(SoundCue);
    AudioComp->bAutoActivate = false;
    AudioComp->AttachToComponent(GetOwner()->GetRootComponent(), 
        FAttachmentTransformRules::KeepWorldTransform);
    
    return AudioComp;
}

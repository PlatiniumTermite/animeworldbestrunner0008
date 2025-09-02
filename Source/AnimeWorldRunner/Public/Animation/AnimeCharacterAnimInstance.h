#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "AnimeRunnerCharacter.h"
#include "AnimeCharacterAnimInstance.generated.h"

UCLASS()
class ANIMEWORLDRUNNER_API UAnimeCharacterAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

public:
    UAnimeCharacterAnimInstance();

protected:
    virtual void NativeInitializeAnimation() override;
    virtual void NativeUpdateAnimation(float DeltaTime) override;

    // Character reference
    UPROPERTY(BlueprintReadOnly, Category = "Character")
    class AAnimeRunnerCharacter* AnimeCharacter;

    // Movement variables
    UPROPERTY(BlueprintReadOnly, Category = "Movement")
    float Speed;

    UPROPERTY(BlueprintReadOnly, Category = "Movement")
    float Direction;

    UPROPERTY(BlueprintReadOnly, Category = "Movement")
    FVector Velocity;

    UPROPERTY(BlueprintReadOnly, Category = "Movement")
    bool bIsInAir;

    UPROPERTY(BlueprintReadOnly, Category = "Movement")
    bool bIsMoving;

    UPROPERTY(BlueprintReadOnly, Category = "Movement")
    bool bIsRunning;

    UPROPERTY(BlueprintReadOnly, Category = "Movement")
    bool bIsWalking;

    // Character states
    UPROPERTY(BlueprintReadOnly, Category = "States")
    bool bIsJumping;

    UPROPERTY(BlueprintReadOnly, Category = "States")
    bool bIsFalling;

    UPROPERTY(BlueprintReadOnly, Category = "States")
    bool bIsGliding;

    UPROPERTY(BlueprintReadOnly, Category = "States")
    bool bIsClimbing;

    UPROPERTY(BlueprintReadOnly, Category = "States")
    bool bIsDashing;

    UPROPERTY(BlueprintReadOnly, Category = "States")
    bool bIsAttacking;

    UPROPERTY(BlueprintReadOnly, Category = "States")
    bool bIsCasting;

    UPROPERTY(BlueprintReadOnly, Category = "States")
    bool bIsHurt;

    UPROPERTY(BlueprintReadOnly, Category = "States")
    bool bIsDead;

    // Animation blend values
    UPROPERTY(BlueprintReadOnly, Category = "Animation")
    float IdleToWalkBlend;

    UPROPERTY(BlueprintReadOnly, Category = "Animation")
    float WalkToRunBlend;

    UPROPERTY(BlueprintReadOnly, Category = "Animation")
    float JumpBlend;

    UPROPERTY(BlueprintReadOnly, Category = "Animation")
    float LandBlend;

    // Anime-style animation properties
    UPROPERTY(BlueprintReadOnly, Category = "Anime Style")
    float AnimationPlayRate;

    UPROPERTY(BlueprintReadOnly, Category = "Anime Style")
    float LeanAmount;

    UPROPERTY(BlueprintReadOnly, Category = "Anime Style")
    FRotator AimRotation;

    UPROPERTY(BlueprintReadOnly, Category = "Anime Style")
    float PitchOffset;

    UPROPERTY(BlueprintReadOnly, Category = "Anime Style")
    float YawOffset;

    // IK (Inverse Kinematics) properties for anime-style foot placement
    UPROPERTY(BlueprintReadOnly, Category = "IK")
    bool bEnableFootIK;

    UPROPERTY(BlueprintReadOnly, Category = "IK")
    float LeftFootIKOffset;

    UPROPERTY(BlueprintReadOnly, Category = "IK")
    float RightFootIKOffset;

    UPROPERTY(BlueprintReadOnly, Category = "IK")
    FRotator LeftFootIKRotation;

    UPROPERTY(BlueprintReadOnly, Category = "IK")
    FRotator RightFootIKRotation;

    // Combat animation properties
    UPROPERTY(BlueprintReadOnly, Category = "Combat")
    int32 AttackComboIndex;

    UPROPERTY(BlueprintReadOnly, Category = "Combat")
    float AttackSpeed;

    UPROPERTY(BlueprintReadOnly, Category = "Combat")
    bool bCanAttackCombo;

    // Facial animation properties (anime-style)
    UPROPERTY(BlueprintReadOnly, Category = "Facial")
    float BlinkTimer;

    UPROPERTY(BlueprintReadOnly, Category = "Facial")
    bool bIsBlinking;

    UPROPERTY(BlueprintReadOnly, Category = "Facial")
    float EyeTrackingAlpha;

    UPROPERTY(BlueprintReadOnly, Category = "Facial")
    FVector2D EyeLookDirection;

private:
    // Helper functions
    void UpdateMovementValues();
    void UpdateCharacterStates();
    void UpdateAnimationBlends(float DeltaTime);
    void UpdateFootIK();
    void UpdateFacialAnimation(float DeltaTime);
    void UpdateAnimeStyleProperties();

    // IK trace functions
    float GetFootIKOffset(FName SocketName);
    FRotator GetFootIKRotation(FName SocketName);

    // Animation timing
    float LastUpdateTime;
    float BlinkInterval;
    float NextBlinkTime;

    // Smoothing values
    float SpeedSmoothRate;
    float DirectionSmoothRate;
    float BlendSmoothRate;
};

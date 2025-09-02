#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/InventoryComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/AudioComponent.h"
#include "AnimeRunnerCharacter.generated.h"

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
    IDLE,
    RUNNING,
    WALKING,
    JUMPING,
    FALLING,
    SLIDING,
    DASHING,
    GLIDING,
    CLIMBING,
    ATTACKING,
    CASTING,
    HURT,
    DEAD
};

UENUM(BlueprintType)
enum class EMovementMode : uint8
{
    GROUND,
    AIR,
    WATER,
    CLIMBING
};

UCLASS()
class ANIMEWORLDRUNNER_API AAnimeRunnerCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AAnimeRunnerCharacter();

protected:
    virtual void BeginPlay() override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:    
    virtual void Tick(float DeltaTime) override;
    
    // 3D Movement functions
    UFUNCTION(BlueprintCallable)
    void MoveForward(float Value);
    
    UFUNCTION(BlueprintCallable)
    void MoveRight(float Value);
    
    UFUNCTION(BlueprintCallable)
    void Jump() override;
    
    UFUNCTION(BlueprintCallable)
    void Dash();
    
    UFUNCTION(BlueprintCallable)
    void StartGliding();
    
    UFUNCTION(BlueprintCallable)
    void StopGliding();
    
    UFUNCTION(BlueprintCallable)
    void StartClimbing();
    
    UFUNCTION(BlueprintCallable)
    void StopClimbing();
    
    UFUNCTION(BlueprintCallable)
    void Attack();
    
    UFUNCTION(BlueprintCallable)
    void CastSpell();
    
    UFUNCTION(BlueprintCallable)
    void UsePower();
    
    // Camera controls
    UFUNCTION(BlueprintCallable)
    void LookUp(float Value);
    
    UFUNCTION(BlueprintCallable)
    void Turn(float Value);
    
    // Getters
    UFUNCTION(BlueprintPure)
    ECharacterState GetCurrentState() const { return CurrentState; }
    
    UFUNCTION(BlueprintPure)
    EMovementMode GetMovementMode() const { return CurrentMovementMode; }
    
    UFUNCTION(BlueprintPure)
    float GetCurrentSpeed() const { return CurrentSpeed; }
    
    UFUNCTION(BlueprintPure)
    bool IsGliding() const { return bIsGliding; }
    
    UFUNCTION(BlueprintPure)
    bool IsClimbing() const { return bIsClimbing; }

protected:
    /** Camera boom positioning the camera behind the character */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    class USpringArmComponent* CameraBoom;

    /** Follow camera */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    class UCameraComponent* FollowCamera;

    // Visual Effects Components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Effects")
    UParticleSystemComponent* DashEffectComponent;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Effects")
    UParticleSystemComponent* JumpEffectComponent;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Effects")
    UParticleSystemComponent* RunEffectComponent;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio")
    UAudioComponent* FootstepAudioComponent;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio")
    UAudioComponent* AbilityAudioComponent;

    // 3D Movement properties
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float WalkSpeed;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float RunSpeed;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float SprintSpeed;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float JumpHeight;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float AirControl;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float DashDistance;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float DashCooldown;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float GlideFallSpeed;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float ClimbSpeed;
    
    // Camera properties
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    float CameraDistance;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    float CameraHeight;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    float CameraSensitivity;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    float CameraLagSpeed;
    
    // Combat properties
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float AttackDamage;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float AttackRange;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float AttackCooldown;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float SpellPower;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float ManaCost;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UInventoryComponent* InventoryComponent;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UAnimeEffectsManager* EffectsManager;
    
    // Character state
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
    ECharacterState CurrentState;
    
    // Movement mode
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
    EMovementMode CurrentMovementMode;
    
    // Current movement speed
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
    float CurrentSpeed;
    
    // Movement states
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
    bool bIsGliding;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
    bool bIsClimbing;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
    bool bIsDashing;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
    bool bIsAttacking;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
    bool bIsCasting;
    
    // Timers
    FTimerHandle DashCooldownTimer;
    FTimerHandle AttackCooldownTimer;
    
    // Input values
    float ForwardInputValue;
    float RightInputValue;
    float LookUpInputValue;
    float TurnInputValue;
    
    // Animation and state handling
    void UpdateAnimationState();
    void UpdateMovementMode();
    void HandleMovement(float DeltaTime);
    void HandleCameraMovement(float DeltaTime);
    void HandleVisualEffects();
    
    // Ability functions
    void PerformDash();
    void UpdateGliding(float DeltaTime);
    void UpdateClimbing(float DeltaTime);
    void PerformAttack();
    void PerformSpellCast();
    
    // Input handling
    void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);
    void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);
    
    // Collision handling
    UFUNCTION()
    void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, 
                        AActor* OtherActor, 
                        UPrimitiveComponent* OtherComp, 
                        int32 OtherBodyIndex, 
                        bool bFromSweep, 
                        const FHitResult& SweepResult);
    
    // Timer callbacks
    UFUNCTION()
    void ResetDashCooldown();
    
    UFUNCTION()
    void ResetAttackCooldown();
    
    // Utility functions
    bool CanDash() const;
    bool CanAttack() const;
    bool CanClimb() const;
    FVector GetClimbDirection() const;

public:
    /** Returns CameraBoom subobject **/
    FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
    /** Returns FollowCamera subobject **/
    FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

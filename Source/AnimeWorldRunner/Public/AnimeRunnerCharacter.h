#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/InventoryComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "AnimeRunnerCharacter.generated.h"

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
    IDLE,
    RUNNING,
    JUMPING,
    SLIDING,
    DASHING,
    HURT,
    DEAD
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
    
    // Movement functions
    UFUNCTION(BlueprintCallable)
    void MoveRight(float Value);
    
    UFUNCTION(BlueprintCallable)
    void Jump() override;
    
    UFUNCTION(BlueprintCallable)
    void Slide();
    
    UFUNCTION(BlueprintCallable)
    void UsePower();
    
    // Getters
    UFUNCTION(BlueprintPure)
    ECharacterState GetCurrentState() const { return CurrentState; }
    
    UFUNCTION(BlueprintPure)
    int32 GetCurrentLane() const { return CurrentLane; }
    
    UFUNCTION(BlueprintPure)
    float GetCurrentSpeed() const { return CurrentSpeed; }

protected:
    /** Camera boom positioning the camera behind the character */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    class USpringArmComponent* CameraBoom;

    /** Follow camera */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    class UCameraComponent* FollowCamera;

    // Movement properties
    UPROPERTY(EditAnywhere, Category = "Movement")
    float BaseMovementSpeed;
    
    UPROPERTY(EditAnywhere, Category = "Movement")
    float MaxMovementSpeed;
    
    UPROPERTY(EditAnywhere, Category = "Movement")
    float LaneChangeSpeed;
    
    UPROPERTY(EditAnywhere, Category = "Movement")
    float SlideDuration;
    
    UPROPERTY(EditAnywhere, Category = "Movement")
    float JumpForce;
    
    UPROPERTY(EditAnywhere, Category = "Movement")
    float LaneWidth;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UInventoryComponent* InventoryComponent;
    
    // Character state
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
    ECharacterState CurrentState;
    
    // Current lane position (-1, 0, 1)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
    int32 CurrentLane;
    
    // Current movement speed
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
    float CurrentSpeed;
    
    // Target position for lane changing
    FVector TargetPosition;
    
    // Is player sliding?
    bool bIsSliding;
    
    // Handle for slide timer
    FTimerHandle SlideTimerHandle;
    
    // Animation handles
    void UpdateAnimationState();
    void HandleLaneChange(float DeltaTime);
    void HandleAutoSpeedIncrease(float DeltaTime);
    
    // Input handling
    void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);
    void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);
    void SwipeLeft();
    void SwipeRight();
    
    // Collision handling
    UFUNCTION()
    void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, 
                        AActor* OtherActor, 
                        UPrimitiveComponent* OtherComp, 
                        int32 OtherBodyIndex, 
                        bool bFromSweep, 
                        const FHitResult& SweepResult);
    
    // Reset slide state
    UFUNCTION()
    void ResetSlide();

public:
    /** Returns CameraBoom subobject **/
    FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
    /** Returns FollowCamera subobject **/
    FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

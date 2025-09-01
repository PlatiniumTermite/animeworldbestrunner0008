#include "AnimeRunnerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/InventoryComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Actors/Collectible.h"
#include "Actors/Obstacle.h"

AAnimeRunnerCharacter::AAnimeRunnerCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
    
    // Set size for mobile
    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
    
    // Don't rotate character to camera direction
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;
    
    // Configure character movement
    GetCharacterMovement()->bOrientRotationToMovement = false;
    GetCharacterMovement()->RotationRate = FRotator(0.f, 0.f, 0.f);
    GetCharacterMovement()->bConstrainToPlane = true;
    GetCharacterMovement()->bSnapToPlaneAtStart = true;
    
    // Create a camera boom (pulls in towards the player if there is a collision)
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character    
    CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

    // Create a follow camera
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
    FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
    
    // Movement defaults
    BaseMovementSpeed = 600.f;
    MaxMovementSpeed = 1200.f;
    LaneChangeSpeed = 10.f;
    SlideDuration = 1.0f;
    JumpForce = 650.f;
    LaneWidth = 200.0f;
    
    // Create inventory component
    InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
    
    // Initial state
    CurrentState = ECharacterState::IDLE;
    CurrentLane = 0;
    CurrentSpeed = BaseMovementSpeed;
    bIsSliding = false;
}

void AAnimeRunnerCharacter::BeginPlay()
{
    Super::BeginPlay();
    
    TargetPosition = GetActorLocation();
    
    // Setup overlap events
    GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AAnimeRunnerCharacter::OnBeginOverlap);
    
    // Set initial movement speed
    GetCharacterMovement()->MaxWalkSpeed = CurrentSpeed;
}

void AAnimeRunnerCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    // Only process movement if running
    if (CurrentState == ECharacterState::RUNNING)
    {
        HandleLaneChange(DeltaTime);
        HandleAutoSpeedIncrease(DeltaTime);
    }
    
    UpdateAnimationState();
}

void AAnimeRunnerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    
    // Set up touch events
    PlayerInputComponent->BindTouch(IE_Pressed, this, &AAnimeRunnerCharacter::TouchStarted);
    PlayerInputComponent->BindTouch(IE_Released, this, &AAnimeRunnerCharacter::TouchStopped);
    
    // Set up keyboard controls for testing
    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
    PlayerInputComponent->BindAction("Slide", IE_Pressed, this, &AAnimeRunnerCharacter::Slide);
    PlayerInputComponent->BindAction("SwipeLeft", IE_Pressed, this, &AAnimeRunnerCharacter::SwipeLeft);
    PlayerInputComponent->BindAction("SwipeRight", IE_Pressed, this, &AAnimeRunnerCharacter::SwipeRight);
    PlayerInputComponent->BindAction("UsePower", IE_Pressed, this, &AAnimeRunnerCharacter::UsePower);
}

void AAnimeRunnerCharacter::MoveRight(float Value)
{
    if (CurrentState != ECharacterState::RUNNING) return;
    
    // Change lanes based on input
    int32 NewLane = FMath::Clamp(CurrentLane + FMath::RoundToInt(Value), -1, 1);
    
    if (NewLane != CurrentLane) 
    {
        CurrentLane = NewLane;
        TargetPosition = FVector(GetActorLocation().X, CurrentLane * LaneWidth, GetActorLocation().Z);
    }
}

void AAnimeRunnerCharacter::Jump()
{
    if (bIsSliding || CurrentState != ECharacterState::RUNNING) return;
    
    Super::Jump();
    CurrentState = ECharacterState::JUMPING;
}

void AAnimeRunnerCharacter::Slide()
{
    if (bIsSliding || CurrentState != ECharacterState::RUNNING || GetMovementComponent()->IsFalling()) return;
    
    bIsSliding = true;
    CurrentState = ECharacterState::SLIDING;
    
    // Adjust capsule height for sliding
    GetCapsuleComponent()->SetCapsuleHalfHeight(44.0f);
    
    // Reset slide after duration
    GetWorldTimerManager().SetTimer(SlideTimerHandle, this, &AAnimeRunnerCharacter::ResetSlide, SlideDuration, false);
}

void AAnimeRunnerCharacter::UsePower()
{
    if (InventoryComponent && InventoryComponent->HasPowerUp())
    {
        // Activate power-up effect
        InventoryComponent->UsePowerUp();
        
        // Implement power-up specific logic here
        // This could be invincibility, speed boost, magnet, etc.
    }
}

void AAnimeRunnerCharacter::UpdateAnimationState()
{
    // Update state based on movement
    if (GetCharacterMovement()->IsFalling())
    {
        CurrentState = ECharacterState::JUMPING;
    }
    else if (bIsSliding)
    {
        CurrentState = ECharacterState::SLIDING;
    }
    else if (GetVelocity().Size() > 0)
    {
        CurrentState = ECharacterState::RUNNING;
    }
    else
    {
        CurrentState = ECharacterState::IDLE;
    }
}

void AAnimeRunnerCharacter::HandleLaneChange(float DeltaTime)
{
    // Smooth lane changing
    FVector CurrentLocation = GetActorLocation();
    FVector NewLocation = FMath::VInterpTo(CurrentLocation, 
        FVector(CurrentLocation.X, TargetPosition.Y, CurrentLocation.Z), 
        DeltaTime, LaneChangeSpeed);
    
    SetActorLocation(NewLocation);
    
    // Auto-run forward
    AddMovementInput(FVector(1.0f, 0.0f, 0.0f), 1.0f);
}

void AAnimeRunnerCharacter::HandleAutoSpeedIncrease(float DeltaTime)
{
    // Gradually increase speed over time
    CurrentSpeed = FMath::FInterpTo(CurrentSpeed, MaxMovementSpeed, DeltaTime, 0.01f);
    GetCharacterMovement()->MaxWalkSpeed = CurrentSpeed;
}

void AAnimeRunnerCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
    // Jump on tap
    Jump();
}

void AAnimeRunnerCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
    // Optional: Handle touch release if needed
}

void AAnimeRunnerCharacter::SwipeLeft()
{
    MoveRight(-1.0f);
}

void AAnimeRunnerCharacter::SwipeRight()
{
    MoveRight(1.0f);
}

void AAnimeRunnerCharacter::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, 
    AActor* OtherActor, 
    UPrimitiveComponent* OtherComp, 
    int32 OtherBodyIndex, 
    bool bFromSweep, 
    const FHitResult& SweepResult)
{
    // Handle collectibles
    if (OtherActor && OtherActor->IsA<AActor>())
    {
        // Check for collectible tag
        if (OtherActor->ActorHasTag(FName("Collectible")))
        {
            // Add to inventory and destroy
            if (InventoryComponent)
            {
                InventoryComponent->AddCoins(10);
            }
            OtherActor->Destroy();
            return;
        }
        
        // Check for obstacle tag
        if (OtherActor->ActorHasTag(FName("Obstacle")))
        {
            // Handle collision with obstacle
            CurrentState = ECharacterState::HURT;
            // Implement game over logic here
            return;
        }
    }
}

void AAnimeRunnerCharacter::ResetSlide()
{
    bIsSliding = false;
    GetCapsuleComponent()->SetCapsuleHalfHeight(88.0f);
    
    if (!GetMovementComponent()->IsFalling())
    {
        CurrentState = ECharacterState::RUNNING;
    }
}

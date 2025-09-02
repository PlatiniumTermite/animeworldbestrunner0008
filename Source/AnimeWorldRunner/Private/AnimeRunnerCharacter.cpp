#include "AnimeRunnerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/InventoryComponent.h"
#include "Effects/AnimeEffectsManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "Actors/Collectible.h"
#include "Actors/Obstacle.h"

AAnimeRunnerCharacter::AAnimeRunnerCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
    
    // Set size for 3D character
    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
    
    // Configure character rotation - allow yaw rotation for 3D movement
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = true;
    bUseControllerRotationRoll = false;
    
    // Configure character movement for 3D anime-style gameplay
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 600.0f, 0.0f);
    GetCharacterMovement()->bConstrainToPlane = false;
    GetCharacterMovement()->bSnapToPlaneAtStart = false;
    
    // Create a camera boom (pulls in towards the player if there is a collision)
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character    
    CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

    // Create a follow camera
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
    FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
    
    // Create visual effects components
    DashEffectComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("DashEffect"));
    DashEffectComponent->SetupAttachment(RootComponent);
    DashEffectComponent->bAutoActivate = false;
    
    JumpEffectComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("JumpEffect"));
    JumpEffectComponent->SetupAttachment(RootComponent);
    JumpEffectComponent->bAutoActivate = false;
    
    RunEffectComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("RunEffect"));
    RunEffectComponent->SetupAttachment(RootComponent);
    RunEffectComponent->bAutoActivate = false;
    
    // Create audio components
    FootstepAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("FootstepAudio"));
    FootstepAudioComponent->SetupAttachment(RootComponent);
    FootstepAudioComponent->bAutoActivate = false;
    
    AbilityAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AbilityAudio"));
    AbilityAudioComponent->SetupAttachment(RootComponent);
    AbilityAudioComponent->bAutoActivate = false;
    
    // Create inventory component
    InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
    
    // Create effects manager
    EffectsManager = CreateDefaultSubobject<UAnimeEffectsManager>(TEXT("EffectsManager"));
    
    // Initialize 3D movement properties
    WalkSpeed = 300.0f;
    RunSpeed = 600.0f;
    SprintSpeed = 900.0f;
    JumpHeight = 600.0f;
    AirControl = 0.8f;
    DashDistance = 1000.0f;
    DashCooldown = 2.0f;
    GlideFallSpeed = 200.0f;
    ClimbSpeed = 400.0f;
    
    // Initialize camera properties
    CameraDistance = 400.0f;
    CameraHeight = 50.0f;
    CameraSensitivity = 1.0f;
    CameraLagSpeed = 3.0f;
    
    // Initialize combat properties
    AttackDamage = 50.0f;
    AttackRange = 200.0f;
    AttackCooldown = 1.0f;
    SpellPower = 100.0f;
    ManaCost = 25.0f;
    
    // Initialize state
    CurrentState = ECharacterState::Idle;
    CurrentMovementMode = EMovementMode::Walking;
    CurrentSpeed = WalkSpeed;
    bIsGliding = false;
    bIsClimbing = false;
    bIsDashing = false;
    bIsAttacking = false;
    bIsCasting = false;
    
    // Initialize input values
    ForwardInputValue = 0.0f;
    RightInputValue = 0.0f;
    LookUpInputValue = 0.0f;
    TurnInputValue = 0.0f;
}

void AAnimeRunnerCharacter::BeginPlay()
{
    Super::BeginPlay();
    
    // Setup camera boom properties
    CameraBoom->TargetArmLength = CameraDistance;
    CameraBoom->SocketOffset = FVector(0.0f, 0.0f, CameraHeight);
    CameraBoom->bDoCollisionTest = true;
    CameraBoom->CameraLagSpeed = CameraLagSpeed;
    CameraBoom->bUseCameraLagSubstepping = true;
    
    // Setup character movement for 3D gameplay
    GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
    GetCharacterMovement()->JumpZVelocity = JumpHeight;
    GetCharacterMovement()->AirControl = AirControl;
    GetCharacterMovement()->GravityScale = 1.0f;
    GetCharacterMovement()->GroundFriction = 8.0f;
    GetCharacterMovement()->BrakingDecelerationWalking = 2048.0f;
    
    // Setup overlap events
    GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AAnimeRunnerCharacter::OnBeginOverlap);
    
    // Set initial movement speed
    GetCharacterMovement()->MaxWalkSpeed = CurrentSpeed;
}

void AAnimeRunnerCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    // Handle 3D movement and abilities
    HandleMovement(DeltaTime);
    HandleCameraMovement(DeltaTime);
    
    // Update special movement modes
    if (bIsGliding)
    {
        UpdateGliding(DeltaTime);
    }
    
    if (bIsClimbing)
    {
        UpdateClimbing(DeltaTime);
    }
    
    // Update animation and visual effects
    UpdateAnimationState();
    UpdateMovementMode();
    HandleVisualEffects();
}

void AAnimeRunnerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    
    // Set up 3D movement key bindings
    PlayerInputComponent->BindAxis("MoveForward", this, &AAnimeRunnerCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &AAnimeRunnerCharacter::MoveRight);
    
    // Camera controls
    PlayerInputComponent->BindAxis("Turn", this, &AAnimeRunnerCharacter::Turn);
    PlayerInputComponent->BindAxis("LookUp", this, &AAnimeRunnerCharacter::LookUp);
    
    // Action bindings
    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AAnimeRunnerCharacter::Jump);
    PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &AAnimeRunnerCharacter::Dash);
    PlayerInputComponent->BindAction("Glide", IE_Pressed, this, &AAnimeRunnerCharacter::StartGliding);
    PlayerInputComponent->BindAction("Glide", IE_Released, this, &AAnimeRunnerCharacter::StopGliding);
    PlayerInputComponent->BindAction("Climb", IE_Pressed, this, &AAnimeRunnerCharacter::StartClimbing);
    PlayerInputComponent->BindAction("Climb", IE_Released, this, &AAnimeRunnerCharacter::StopClimbing);
    PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &AAnimeRunnerCharacter::Attack);
    PlayerInputComponent->BindAction("CastSpell", IE_Pressed, this, &AAnimeRunnerCharacter::CastSpell);
    PlayerInputComponent->BindAction("UsePower", IE_Pressed, this, &AAnimeRunnerCharacter::UsePower);
    
    // Touch input
    PlayerInputComponent->BindTouch(IE_Pressed, this, &AAnimeRunnerCharacter::TouchStarted);
    PlayerInputComponent->BindTouch(IE_Released, this, &AAnimeRunnerCharacter::TouchStopped);
}

void AAnimeRunnerCharacter::MoveForward(float Value)
{
    ForwardInputValue = Value;
    
    if (bIsClimbing || bIsDashing) return;
    
    // Add movement input
    AddMovementInput(GetActorForwardVector(), Value);
}

void AAnimeRunnerCharacter::MoveRight(float Value)
{
    RightInputValue = Value;
    
    if (bIsClimbing || bIsDashing) return;
    
    // Add movement input
    AddMovementInput(GetActorRightVector(), Value);
}

void AAnimeRunnerCharacter::Jump()
{
    if (bIsClimbing)
    {
        // Jump off wall
        StopClimbing();
        FVector JumpDirection = -GetActorForwardVector() + FVector(0, 0, 1);
        LaunchCharacter(JumpDirection * JumpHeight * 0.8f, false, true);
        CurrentState = ECharacterState::Jumping;
        
        // Play jump effects
        if (EffectsManager)
        {
            EffectsManager->PlayEffect(EAnimeEffectType::Jump, GetActorLocation());
        }
    }
    else if (GetCharacterMovement()->IsMovingOnGround())
    {
        Super::Jump();
        CurrentState = ECharacterState::Jumping;
        
        // Play jump effects
        if (EffectsManager)
        {
            EffectsManager->PlayEffect(EAnimeEffectType::Jump, GetActorLocation());
        }
    }
}

void AAnimeRunnerCharacter::Dash()
{
    if (CanDash())
    {
        PerformDash();
    }
}

void AAnimeRunnerCharacter::StartGliding()
{
    if (!GetCharacterMovement()->IsMovingOnGround() && !bIsGliding)
    {
        bIsGliding = true;
        CurrentState = ECharacterState::Gliding;
        GetCharacterMovement()->GravityScale = 0.3f;
    }
}

void AAnimeRunnerCharacter::StopGliding()
{
    if (bIsGliding)
    {
        bIsGliding = false;
        GetCharacterMovement()->GravityScale = 1.0f;
        
        if (GetCharacterMovement()->IsMovingOnGround())
        {
            CurrentState = ECharacterState::Running;
        }
        else
        {
            CurrentState = ECharacterState::Falling;
        }
    }
}

void AAnimeRunnerCharacter::StartClimbing()
{
    if (CanClimb())
    {
        bIsClimbing = true;
        CurrentState = ECharacterState::Climbing;
        GetCharacterMovement()->SetMovementMode(MOVE_Flying);
    }
}

void AAnimeRunnerCharacter::StopClimbing()
{
    if (bIsClimbing)
    {
        bIsClimbing = false;
        GetCharacterMovement()->SetMovementMode(MOVE_Walking);
        CurrentState = ECharacterState::Falling;
    }
}

void AAnimeRunnerCharacter::Attack()
{
    if (CanAttack())
    {
        PerformAttack();
    }
}

void AAnimeRunnerCharacter::CastSpell()
{
    if (!bIsCasting && !bIsAttacking)
    {
        PerformSpellCast();
    }
}

void AAnimeRunnerCharacter::LookUp(float Value)
{
    LookUpInputValue = Value;
    AddControllerPitchInput(Value * CameraSensitivity);
}

void AAnimeRunnerCharacter::Turn(float Value)
{
    TurnInputValue = Value;
    AddControllerYawInput(Value * CameraSensitivity);
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

void AAnimeRunnerCharacter::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (ACollectible* Collectible = Cast<ACollectible>(OtherActor))
    {
        if (InventoryComponent)
        {
            InventoryComponent->AddCoins(Collectible->GetValue());
            
            // Play collection effects
            if (EffectsManager)
            {
                EffectsManager->PlayEffect(EAnimeEffectType::Collect, Collectible->GetActorLocation());
                
                // Special power-up effect for certain collectibles
                if (Collectible->GetCollectibleType() == ECollectibleType::PowerUp)
                {
                    EffectsManager->PlayEffect(EAnimeEffectType::PowerUp, GetActorLocation());
                    EffectsManager->PlayAuraEffect(true, FLinearColor::Yellow);
                    
                    // Remove aura after power-up duration
                    FTimerHandle AuraTimer;
                    GetWorld()->GetTimerManager().SetTimer(AuraTimer, [this]()
                    {
                        if (EffectsManager)
                        {
                            EffectsManager->PlayAuraEffect(false);
                        }
                    }, 10.0f, false);
                }
            }
            
            Collectible->Destroy();
        }
    }
    else if (AObstacle* Obstacle = Cast<AObstacle>(OtherActor))
    {
        // Handle obstacle collision with effects
        if (EffectsManager)
        {
            EffectsManager->PlayImpactEffect(GetActorLocation(), (GetActorLocation() - Obstacle->GetActorLocation()).GetSafeNormal());
            EffectsManager->PlayScreenFlash(FLinearColor::Red, 0.2f);
        }
        
        // Set hurt state
        CurrentState = ECharacterState::Hurt;
        
        // Reset to normal state after brief moment
        FTimerHandle HurtTimer;
        GetWorld()->GetTimerManager().SetTimer(HurtTimer, [this]()
        {
            if (GetCharacterMovement()->IsMovingOnGround())
            {
                CurrentState = ECharacterState::Running;
            }
        }, 0.5f, false);
        
        UE_LOG(LogTemp, Warning, TEXT("Hit obstacle!"));
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

void AAnimeRunnerCharacter::HandleMovement(float DeltaTime)
{
    // Update movement speed based on input
    float InputMagnitude = FMath::Sqrt(ForwardInputValue * ForwardInputValue + RightInputValue * RightInputValue);
    
    if (InputMagnitude > 0.1f)
    {
        // Determine movement mode based on input magnitude
        if (InputMagnitude > 0.8f)
        {
            CurrentMovementMode = EMovementMode::Running;
            CurrentSpeed = RunSpeed;
        }
        else if (InputMagnitude > 0.4f)
        {
            CurrentMovementMode = EMovementMode::Walking;
            CurrentSpeed = WalkSpeed;
        }
        else
        {
            CurrentMovementMode = EMovementMode::Walking;
            CurrentSpeed = WalkSpeed * 0.5f;
        }
        
        GetCharacterMovement()->MaxWalkSpeed = CurrentSpeed;
        
        if (GetCharacterMovement()->IsMovingOnGround())
        {
            CurrentState = ECharacterState::Running;
        }
    }
    else
    {
        CurrentMovementMode = EMovementMode::Walking;
        if (GetCharacterMovement()->IsMovingOnGround())
        {
            CurrentState = ECharacterState::Idle;
        }
    }
}

void AAnimeRunnerCharacter::HandleCameraMovement(float DeltaTime)
{
    // Smooth camera lag for anime-style feel
    if (CameraBoom)
    {
        CameraBoom->CameraLagSpeed = CameraLagSpeed;
        CameraBoom->TargetArmLength = FMath::FInterpTo(CameraBoom->TargetArmLength, CameraDistance, DeltaTime, 2.0f);
    }
}

void AAnimeRunnerCharacter::UpdateMovementMode()
{
    // Update character state based on movement
    if (GetCharacterMovement()->IsFalling())
    {
        if (bIsGliding)
        {
            CurrentState = ECharacterState::Gliding;
        }
        else
        {
            CurrentState = ECharacterState::Falling;
        }
    }
    else if (GetCharacterMovement()->IsMovingOnGround())
    {
        if (GetVelocity().Size() > 10.0f)
        {
            CurrentState = ECharacterState::Running;
        }
        else
        {
            CurrentState = ECharacterState::Idle;
        }
    }
}

void AAnimeRunnerCharacter::HandleVisualEffects()
{
    if (!EffectsManager) return;
    
    // Handle running effects
    if (CurrentState == ECharacterState::Running && GetVelocity().Size() > 100.0f)
    {
        EffectsManager->PlayEffectAttached(EAnimeEffectType::Running, GetRootComponent());
    }
    else
    {
        EffectsManager->StopEffect(EAnimeEffectType::Running);
    }
    
    // Handle gliding effects
    if (bIsGliding)
    {
        EffectsManager->PlayEffectAttached(EAnimeEffectType::Glide, GetRootComponent());
    }
    else
    {
        EffectsManager->StopEffect(EAnimeEffectType::Glide);
    }
    
    // Handle climbing effects
    if (bIsClimbing)
    {
        EffectsManager->PlayEffectAttached(EAnimeEffectType::Climb, GetRootComponent());
    }
    else
    {
        EffectsManager->StopEffect(EAnimeEffectType::Climb);
    }
    
    // Handle landing effects
    static bool bWasFalling = false;
    bool bIsFalling = GetCharacterMovement()->IsFalling();
    
    if (bWasFalling && !bIsFalling && GetCharacterMovement()->IsMovingOnGround())
    {
        // Just landed
        EffectsManager->PlayEffect(EAnimeEffectType::Landing, GetActorLocation());
    }
    
    bWasFalling = bIsFalling;
}

void AAnimeRunnerCharacter::UpdateAnimationState()
{
    // This function will be used to update animation blueprints
    // based on the current character state and movement mode
    
    // Animation state is now handled by UpdateMovementMode()
    // This can be extended for specific animation blueprint integration
}

void AAnimeRunnerCharacter::PerformDash()
{
    if (!bIsDashing)
    {
        bIsDashing = true;
        CurrentState = ECharacterState::Dashing;
        
        // Get dash direction
        FVector DashDirection = GetActorForwardVector();
        if (RightInputValue != 0.0f || ForwardInputValue != 0.0f)
        {
            DashDirection = (GetActorForwardVector() * ForwardInputValue + GetActorRightVector() * RightInputValue).GetSafeNormal();
        }
        
        // Perform dash
        LaunchCharacter(DashDirection * DashDistance, true, false);
        
        // Play dash effects
        if (EffectsManager)
        {
            EffectsManager->PlayEffect(EAnimeEffectType::Dash, GetActorLocation());
            EffectsManager->PlayDashTrail(GetActorLocation(), GetActorLocation() + DashDirection * DashDistance);
        }
        
        // Set cooldown timer
        GetWorldTimerManager().SetTimer(DashCooldownTimer, this, &AAnimeRunnerCharacter::ResetDashCooldown, DashCooldown, false);
        
        // Reset dash state after short duration
        FTimerHandle DashDurationTimer;
        GetWorldTimerManager().SetTimer(DashDurationTimer, [this]()
        {
            bIsDashing = false;
            if (GetCharacterMovement()->IsMovingOnGround())
            {
                CurrentState = ECharacterState::Running;
            }
        }, 0.3f, false);
    }
}

void AAnimeRunnerCharacter::UpdateGliding(float DeltaTime)
{
    if (bIsGliding)
    {
        // Apply gliding physics
        FVector Velocity = GetVelocity();
        Velocity.Z = FMath::Max(Velocity.Z, -GlideFallSpeed);
        GetCharacterMovement()->Velocity = Velocity;
        
        // Add forward momentum while gliding
        if (ForwardInputValue > 0.0f)
        {
            AddMovementInput(GetActorForwardVector(), ForwardInputValue * 0.5f);
        }
    }
}

void AAnimeRunnerCharacter::UpdateClimbing(float DeltaTime)
{
    if (bIsClimbing)
    {
        // Handle climbing movement
        FVector ClimbDirection = GetClimbDirection();
        
        if (ForwardInputValue > 0.0f)
        {
            AddMovementInput(ClimbDirection, ForwardInputValue * ClimbSpeed * DeltaTime);
        }
        
        if (LookUpInputValue != 0.0f)
        {
            AddMovementInput(FVector::UpVector, LookUpInputValue * ClimbSpeed * DeltaTime);
        }
    }
}

void AAnimeRunnerCharacter::PerformAttack()
{
    if (!bIsAttacking)
    {
        bIsAttacking = true;
        CurrentState = ECharacterState::Attacking;
        
        // Play attack effects
        if (EffectsManager)
        {
            EffectsManager->PlayEffectAttached(EAnimeEffectType::Attack, GetRootComponent(), FName("hand_r"));
        }
        
        // Set attack cooldown
        GetWorldTimerManager().SetTimer(AttackCooldownTimer, this, &AAnimeRunnerCharacter::ResetAttackCooldown, AttackCooldown, false);
        
        // Reset attack state after animation duration
        FTimerHandle AttackDurationTimer;
        GetWorldTimerManager().SetTimer(AttackDurationTimer, [this]()
        {
            bIsAttacking = false;
            CurrentState = ECharacterState::Running;
        }, 0.8f, false);
    }
}

void AAnimeRunnerCharacter::PerformSpellCast()
{
    if (!bIsCasting)
    {
        bIsCasting = true;
        CurrentState = ECharacterState::Casting;
        
        // Play spell casting effects
        if (EffectsManager)
        {
            EffectsManager->PlayEffectAttached(EAnimeEffectType::SpellCast, GetRootComponent(), FName("hand_l"));
        }
        
        // Reset casting state after spell duration
        FTimerHandle CastDurationTimer;
        GetWorldTimerManager().SetTimer(CastDurationTimer, [this]()
        {
            bIsCasting = false;
            CurrentState = ECharacterState::Running;
        }, 1.2f, false);
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

void AAnimeRunnerCharacter::ResetDashCooldown()
{
    // Dash is now available again
}

void AAnimeRunnerCharacter::ResetAttackCooldown()
{
    // Attack is now available again
}

bool AAnimeRunnerCharacter::CanDash() const
{
    return !bIsDashing && !GetWorldTimerManager().IsTimerActive(DashCooldownTimer);
}

bool AAnimeRunnerCharacter::CanAttack() const
{
    return !bIsAttacking && !bIsCasting && !GetWorldTimerManager().IsTimerActive(AttackCooldownTimer);
}

bool AAnimeRunnerCharacter::CanClimb() const
{
    // Check if there's a climbable surface nearby
    FVector Start = GetActorLocation();
    FVector End = Start + GetActorForwardVector() * 100.0f;
    
    FHitResult HitResult;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this);
    
    return GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_WorldStatic, QueryParams);
}

FVector AAnimeRunnerCharacter::GetClimbDirection() const
{
    // Get the normal of the surface we're climbing
    FVector Start = GetActorLocation();
    FVector End = Start + GetActorForwardVector() * 100.0f;
    
    FHitResult HitResult;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this);
    
    if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_WorldStatic, QueryParams))
    {
        return -HitResult.Normal;
    }
    
    return GetActorForwardVector();
}

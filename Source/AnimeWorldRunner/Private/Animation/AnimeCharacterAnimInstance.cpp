#include "Animation/AnimeCharacterAnimInstance.h"
#include "AnimeRunnerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "Components/CapsuleComponent.h"

UAnimeCharacterAnimInstance::UAnimeCharacterAnimInstance()
{
    // Initialize default values
    Speed = 0.0f;
    Direction = 0.0f;
    bIsInAir = false;
    bIsMoving = false;
    bIsRunning = false;
    bIsWalking = false;
    bIsJumping = false;
    bIsFalling = false;
    bIsGliding = false;
    bIsClimbing = false;
    bIsDashing = false;
    bIsAttacking = false;
    bIsCasting = false;
    bIsHurt = false;
    bIsDead = false;
    
    // Animation blend values
    IdleToWalkBlend = 0.0f;
    WalkToRunBlend = 0.0f;
    JumpBlend = 0.0f;
    LandBlend = 0.0f;
    
    // Anime-style properties
    AnimationPlayRate = 1.0f;
    LeanAmount = 0.0f;
    PitchOffset = 0.0f;
    YawOffset = 0.0f;
    
    // IK properties
    bEnableFootIK = true;
    LeftFootIKOffset = 0.0f;
    RightFootIKOffset = 0.0f;
    
    // Combat properties
    AttackComboIndex = 0;
    AttackSpeed = 1.0f;
    bCanAttackCombo = false;
    
    // Facial animation
    BlinkTimer = 0.0f;
    bIsBlinking = false;
    EyeTrackingAlpha = 0.0f;
    
    // Smoothing rates
    SpeedSmoothRate = 10.0f;
    DirectionSmoothRate = 15.0f;
    BlendSmoothRate = 8.0f;
    
    // Blink timing
    BlinkInterval = 3.0f;
    NextBlinkTime = BlinkInterval;
}

void UAnimeCharacterAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();
    
    // Cache character reference
    AnimeCharacter = Cast<AAnimeRunnerCharacter>(TryGetPawnOwner());
    
    LastUpdateTime = GetWorld()->GetTimeSeconds();
}

void UAnimeCharacterAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
    Super::NativeUpdateAnimation(DeltaTime);
    
    if (!AnimeCharacter) return;
    
    // Update all animation properties
    UpdateMovementValues();
    UpdateCharacterStates();
    UpdateAnimationBlends(DeltaTime);
    UpdateFootIK();
    UpdateFacialAnimation(DeltaTime);
    UpdateAnimeStyleProperties();
    
    LastUpdateTime = GetWorld()->GetTimeSeconds();
}

void UAnimeCharacterAnimInstance::UpdateMovementValues()
{
    if (!AnimeCharacter) return;
    
    // Get movement component
    UCharacterMovementComponent* MovementComp = AnimeCharacter->GetCharacterMovement();
    if (!MovementComp) return;
    
    // Update velocity and speed
    Velocity = MovementComp->Velocity;
    float NewSpeed = Velocity.Size2D();
    Speed = FMath::FInterpTo(Speed, NewSpeed, GetWorld()->GetDeltaSeconds(), SpeedSmoothRate);
    
    // Update movement booleans
    bIsMoving = Speed > 10.0f;
    bIsWalking = Speed > 10.0f && Speed <= 300.0f;
    bIsRunning = Speed > 300.0f;
    bIsInAir = MovementComp->IsFalling();
    
    // Calculate movement direction relative to character
    if (bIsMoving)
    {
        FVector ForwardVector = AnimeCharacter->GetActorForwardVector();
        FVector RightVector = AnimeCharacter->GetActorRightVector();
        
        FVector NormalizedVelocity = Velocity.GetSafeNormal2D();
        
        float ForwardDot = FVector::DotProduct(ForwardVector, NormalizedVelocity);
        float RightDot = FVector::DotProduct(RightVector, NormalizedVelocity);
        
        float NewDirection = FMath::Atan2(RightDot, ForwardDot) * (180.0f / PI);
        Direction = FMath::FInterpTo(Direction, NewDirection, GetWorld()->GetDeltaSeconds(), DirectionSmoothRate);
    }
}

void UAnimeCharacterAnimInstance::UpdateCharacterStates()
{
    if (!AnimeCharacter) return;
    
    // Get current character state
    ECharacterState CurrentState = AnimeCharacter->GetCurrentState();
    
    // Update state booleans
    bIsJumping = (CurrentState == ECharacterState::Jumping);
    bIsFalling = (CurrentState == ECharacterState::Falling);
    bIsGliding = AnimeCharacter->IsGliding();
    bIsClimbing = AnimeCharacter->IsClimbing();
    bIsDashing = (CurrentState == ECharacterState::Dashing);
    bIsAttacking = (CurrentState == ECharacterState::Attacking);
    bIsCasting = (CurrentState == ECharacterState::Casting);
    bIsHurt = (CurrentState == ECharacterState::Hurt);
    bIsDead = (CurrentState == ECharacterState::Dead);
}

void UAnimeCharacterAnimInstance::UpdateAnimationBlends(float DeltaTime)
{
    // Smooth blend transitions for anime-style animation
    float TargetIdleToWalk = bIsWalking ? 1.0f : 0.0f;
    IdleToWalkBlend = FMath::FInterpTo(IdleToWalkBlend, TargetIdleToWalk, DeltaTime, BlendSmoothRate);
    
    float TargetWalkToRun = bIsRunning ? 1.0f : 0.0f;
    WalkToRunBlend = FMath::FInterpTo(WalkToRunBlend, TargetWalkToRun, DeltaTime, BlendSmoothRate);
    
    float TargetJump = bIsJumping ? 1.0f : 0.0f;
    JumpBlend = FMath::FInterpTo(JumpBlend, TargetJump, DeltaTime, BlendSmoothRate * 2.0f);
    
    // Landing blend - triggered when transitioning from air to ground
    static bool bWasInAir = false;
    if (bWasInAir && !bIsInAir)
    {
        LandBlend = 1.0f;
    }
    else
    {
        LandBlend = FMath::FInterpTo(LandBlend, 0.0f, DeltaTime, BlendSmoothRate * 3.0f);
    }
    bWasInAir = bIsInAir;
}

void UAnimeCharacterAnimInstance::UpdateFootIK()
{
    if (!bEnableFootIK || !AnimeCharacter) return;
    
    // Update foot IK offsets for anime-style ground adaptation
    LeftFootIKOffset = GetFootIKOffset(FName("foot_l"));
    RightFootIKOffset = GetFootIKOffset(FName("foot_r"));
    
    LeftFootIKRotation = GetFootIKRotation(FName("foot_l"));
    RightFootIKRotation = GetFootIKRotation(FName("foot_r"));
}

void UAnimeCharacterAnimInstance::UpdateFacialAnimation(float DeltaTime)
{
    // Update blink animation
    BlinkTimer += DeltaTime;
    
    if (BlinkTimer >= NextBlinkTime)
    {
        bIsBlinking = true;
        BlinkTimer = 0.0f;
        NextBlinkTime = FMath::RandRange(2.0f, 5.0f); // Random blink interval
        
        // Reset blink after short duration
        FTimerHandle BlinkResetTimer;
        GetWorld()->GetTimerManager().SetTimer(BlinkResetTimer, [this]()
        {
            bIsBlinking = false;
        }, 0.15f, false);
    }
    
    // Update eye tracking (look at camera or movement direction)
    if (AnimeCharacter && bIsMoving)
    {
        FVector MovementDirection = Velocity.GetSafeNormal2D();
        FVector ForwardVector = AnimeCharacter->GetActorForwardVector();
        
        float DotProduct = FVector::DotProduct(ForwardVector, MovementDirection);
        EyeTrackingAlpha = FMath::FInterpTo(EyeTrackingAlpha, DotProduct, DeltaTime, 2.0f);
        
        // Calculate eye look direction
        FVector RightVector = AnimeCharacter->GetActorRightVector();
        float RightDot = FVector::DotProduct(RightVector, MovementDirection);
        
        EyeLookDirection.X = FMath::FInterpTo(EyeLookDirection.X, RightDot * 0.5f, DeltaTime, 3.0f);
        EyeLookDirection.Y = FMath::FInterpTo(EyeLookDirection.Y, DotProduct * 0.3f, DeltaTime, 3.0f);
    }
}

void UAnimeCharacterAnimInstance::UpdateAnimeStyleProperties()
{
    if (!AnimeCharacter) return;
    
    // Update animation play rate based on movement speed for anime-style feel
    if (bIsRunning)
    {
        AnimationPlayRate = FMath::GetMappedRangeValueClamped(FVector2D(300.0f, 900.0f), FVector2D(1.0f, 1.5f), Speed);
    }
    else if (bIsWalking)
    {
        AnimationPlayRate = FMath::GetMappedRangeValueClamped(FVector2D(10.0f, 300.0f), FVector2D(0.8f, 1.0f), Speed);
    }
    else
    {
        AnimationPlayRate = 1.0f;
    }
    
    // Calculate lean amount for anime-style movement
    if (bIsMoving && !bIsInAir)
    {
        float TargetLean = Direction * 0.3f; // Lean into turns
        LeanAmount = FMath::FInterpTo(LeanAmount, TargetLean, GetWorld()->GetDeltaSeconds(), 5.0f);
    }
    else
    {
        LeanAmount = FMath::FInterpTo(LeanAmount, 0.0f, GetWorld()->GetDeltaSeconds(), 8.0f);
    }
    
    // Update aim rotation for combat animations
    if (AnimeCharacter->GetController())
    {
        AimRotation = AnimeCharacter->GetController()->GetControlRotation();
        FRotator ActorRotation = AnimeCharacter->GetActorRotation();
        
        FRotator DeltaRotation = UKismetMathLibrary::NormalizedDeltaRotator(AimRotation, ActorRotation);
        
        PitchOffset = FMath::FInterpTo(PitchOffset, DeltaRotation.Pitch, GetWorld()->GetDeltaSeconds(), 10.0f);
        YawOffset = FMath::FInterpTo(YawOffset, DeltaRotation.Yaw, GetWorld()->GetDeltaSeconds(), 10.0f);
    }
    
    // Update combat properties
    if (bIsAttacking)
    {
        AttackSpeed = 1.2f; // Faster attack animations for anime style
        bCanAttackCombo = true;
    }
    else
    {
        AttackSpeed = 1.0f;
        
        // Reset combo after delay
        static float ComboResetTimer = 0.0f;
        ComboResetTimer += GetWorld()->GetDeltaSeconds();
        
        if (ComboResetTimer > 1.0f)
        {
            AttackComboIndex = 0;
            bCanAttackCombo = false;
            ComboResetTimer = 0.0f;
        }
    }
}

float UAnimeCharacterAnimInstance::GetFootIKOffset(FName SocketName)
{
    if (!AnimeCharacter) return 0.0f;
    
    // Get foot socket location
    FVector SocketLocation = AnimeCharacter->GetMesh()->GetSocketLocation(SocketName);
    
    // Trace down to find ground
    FVector TraceStart = SocketLocation;
    FVector TraceEnd = TraceStart - FVector(0, 0, 50.0f); // Trace 50cm down
    
    FHitResult HitResult;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(AnimeCharacter);
    
    if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_WorldStatic, QueryParams))
    {
        // Calculate offset needed to place foot on ground
        float CapsuleHalfHeight = AnimeCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
        float GroundZ = HitResult.Location.Z;
        float CharacterZ = AnimeCharacter->GetActorLocation().Z - CapsuleHalfHeight;
        
        return GroundZ - CharacterZ;
    }
    
    return 0.0f;
}

FRotator UAnimeCharacterAnimInstance::GetFootIKRotation(FName SocketName)
{
    if (!AnimeCharacter) return FRotator::ZeroRotator;
    
    // Get foot socket location
    FVector SocketLocation = AnimeCharacter->GetMesh()->GetSocketLocation(SocketName);
    
    // Trace down to find ground normal
    FVector TraceStart = SocketLocation;
    FVector TraceEnd = TraceStart - FVector(0, 0, 50.0f);
    
    FHitResult HitResult;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(AnimeCharacter);
    
    if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_WorldStatic, QueryParams))
    {
        // Calculate rotation to align foot with ground normal
        FVector GroundNormal = HitResult.Normal;
        FVector FootForward = AnimeCharacter->GetActorForwardVector();
        FVector FootRight = FVector::CrossProduct(GroundNormal, FootForward).GetSafeNormal();
        FootForward = FVector::CrossProduct(FootRight, GroundNormal).GetSafeNormal();
        
        return UKismetMathLibrary::MakeRotFromXZ(FootForward, GroundNormal);
    }
    
    return FRotator::ZeroRotator;
}

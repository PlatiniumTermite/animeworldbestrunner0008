#include "Actors/Collectible.h"
#include "AnimeRunnerCharacter.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InventoryComponent.h"
#include "Kismet/GameplayStatics.h"

ACollectible::ACollectible()
{
    PrimaryActorTick.bCanEverTick = true;
    
    // Create collision sphere
    CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
    RootComponent = CollisionSphere;
    CollisionSphere->SetSphereRadius(50.0f);
    CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    CollisionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
    CollisionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    
    // Create mesh component
    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    MeshComponent->SetupAttachment(RootComponent);
    MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    
    // Set default values
    CollectibleType = ECollectibleType::COIN;
    Value = 10;
    bRotates = true;
    RotationSpeed = 90.0f;
    bFloats = true;
    FloatAmplitude = 20.0f;
    FloatSpeed = 2.0f;
    
    TimeAccumulator = 0.0f;
    
    // Add collectible tag
    Tags.Add(FName("Collectible"));
}

void ACollectible::BeginPlay()
{
    Super::BeginPlay();
    
    InitialZ = GetActorLocation().Z;
    
    // Bind overlap event
    CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &ACollectible::OnBeginOverlap);
}

void ACollectible::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    TimeAccumulator += DeltaTime;
    
    FVector CurrentLocation = GetActorLocation();
    FRotator CurrentRotation = GetActorRotation();
    
    // Handle rotation
    if (bRotates)
    {
        CurrentRotation.Yaw += RotationSpeed * DeltaTime;
        SetActorRotation(CurrentRotation);
    }
    
    // Handle floating animation
    if (bFloats)
    {
        float NewZ = InitialZ + FMath::Sin(TimeAccumulator * FloatSpeed) * FloatAmplitude;
        CurrentLocation.Z = NewZ;
        SetActorLocation(CurrentLocation);
    }
}

void ACollectible::Collect(AAnimeRunnerCharacter* Player)
{
    if (!Player)
    {
        return;
    }
    
    UInventoryComponent* Inventory = Player->FindComponentByClass<UInventoryComponent>();
    if (Inventory)
    {
        switch (CollectibleType)
        {
            case ECollectibleType::COIN:
                Inventory->AddCoins(Value);
                break;
            case ECollectibleType::POWER_UP:
                // Add power-up logic here
                break;
            case ECollectibleType::SPECIAL_ITEM:
                // Add special item logic here
                break;
        }
    }
    
    // Play collection effect/sound here
    
    // Destroy or return to pool
    Destroy();
}

void ACollectible::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, 
    AActor* OtherActor, 
    UPrimitiveComponent* OtherComp, 
    int32 OtherBodyIndex, 
    bool bFromSweep, 
    const FHitResult& SweepResult)
{
    if (AAnimeRunnerCharacter* Player = Cast<AAnimeRunnerCharacter>(OtherActor))
    {
        Collect(Player);
    }
}

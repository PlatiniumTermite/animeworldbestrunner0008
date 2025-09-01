#include "Actors/Obstacle.h"
#include "AnimeRunnerCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameModes/AWRGameModeBase.h"
#include "Kismet/GameplayStatics.h"

AObstacle::AObstacle()
{
    PrimaryActorTick.bCanEverTick = true;
    
    // Create collision box
    CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
    RootComponent = CollisionBox;
    CollisionBox->SetBoxExtent(FVector(50.0f, 50.0f, 100.0f));
    CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    CollisionBox->SetCollisionResponseToAllChannels(ECR_Block);
    CollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    
    // Create mesh component
    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    MeshComponent->SetupAttachment(RootComponent);
    MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    
    // Set default values
    ObstacleType = EObstacleType::STATIC;
    bCanBeDestroyed = false;
    Damage = 1;
    
    // Movement defaults
    bMoves = false;
    MovementDirection = FVector(0.0f, 1.0f, 0.0f);
    MovementSpeed = 100.0f;
    MovementRange = 200.0f;
    MovementDistance = 0.0f;
    bMovingForward = true;
    
    // Add obstacle tag
    Tags.Add(FName("Obstacle"));
}

void AObstacle::BeginPlay()
{
    Super::BeginPlay();
    
    InitialPosition = GetActorLocation();
    
    // Bind overlap event
    CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AObstacle::OnBeginOverlap);
}

void AObstacle::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    if (bMoves)
    {
        HandleMovement(DeltaTime);
    }
}

void AObstacle::Hit(AAnimeRunnerCharacter* Player)
{
    if (!Player)
    {
        return;
    }
    
    // Get game mode to handle game over
    if (AAWRGameModeBase* GameMode = Cast<AAWRGameModeBase>(UGameplayStatics::GetGameMode(this)))
    {
        GameMode->EndGame();
    }
    
    // Play hit effect/sound here
    
    // If destructible, destroy the obstacle
    if (bCanBeDestroyed)
    {
        Destroy();
    }
}

void AObstacle::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, 
    AActor* OtherActor, 
    UPrimitiveComponent* OtherComp, 
    int32 OtherBodyIndex, 
    bool bFromSweep, 
    const FHitResult& SweepResult)
{
    if (AAnimeRunnerCharacter* Player = Cast<AAnimeRunnerCharacter>(OtherActor))
    {
        Hit(Player);
    }
}

void AObstacle::HandleMovement(float DeltaTime)
{
    FVector CurrentLocation = GetActorLocation();
    FVector MovementDelta = MovementDirection * MovementSpeed * DeltaTime;
    
    if (bMovingForward)
    {
        MovementDistance += MovementSpeed * DeltaTime;
        if (MovementDistance >= MovementRange)
        {
            bMovingForward = false;
            MovementDistance = MovementRange;
        }
    }
    else
    {
        MovementDistance -= MovementSpeed * DeltaTime;
        if (MovementDistance <= 0.0f)
        {
            bMovingForward = true;
            MovementDistance = 0.0f;
        }
    }
    
    // Calculate new position
    FVector NewLocation = InitialPosition + (MovementDirection * MovementDistance);
    SetActorLocation(NewLocation);
}

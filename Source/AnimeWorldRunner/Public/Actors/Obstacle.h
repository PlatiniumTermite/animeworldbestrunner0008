#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Obstacle.generated.h"

UENUM(BlueprintType)
enum class EObstacleType : uint8
{
    STATIC,
    MOVING,
    DESTRUCTIBLE
};

UCLASS()
class ANIMEWORLDRUNNER_API AObstacle : public AActor
{
    GENERATED_BODY()
    
public:    
    AObstacle();

protected:
    virtual void BeginPlay() override;

public:    
    virtual void Tick(float DeltaTime) override;
    
    // Called when hit by player
    UFUNCTION(BlueprintCallable)
    void Hit(class AAnimeRunnerCharacter* Player);
    
    // Get obstacle properties
    UFUNCTION(BlueprintPure)
    EObstacleType GetObstacleType() const { return ObstacleType; }
    
    UFUNCTION(BlueprintPure)
    bool CanBeDestroyed() const { return bCanBeDestroyed; }

protected:
    // Components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UBoxComponent* CollisionBox;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* MeshComponent;
    
    // Obstacle properties
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obstacle")
    EObstacleType ObstacleType;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obstacle")
    bool bCanBeDestroyed;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obstacle")
    int32 Damage;
    
    // Movement properties (for moving obstacles)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    bool bMoves;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    FVector MovementDirection;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float MovementSpeed;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float MovementRange;
    
    // Movement variables
    FVector InitialPosition;
    float MovementDistance;
    bool bMovingForward;
    
    // Collision handling
    UFUNCTION()
    void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, 
                        AActor* OtherActor, 
                        UPrimitiveComponent* OtherComp, 
                        int32 OtherBodyIndex, 
                        bool bFromSweep, 
                        const FHitResult& SweepResult);
    
    // Handle movement
    void HandleMovement(float DeltaTime);
};

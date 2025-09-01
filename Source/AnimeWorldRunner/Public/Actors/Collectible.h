#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Collectible.generated.h"

UENUM(BlueprintType)
enum class ECollectibleType : uint8
{
    COIN,
    POWER_UP,
    SPECIAL_ITEM
};

UCLASS()
class ANIMEWORLDRUNNER_API ACollectible : public AActor
{
    GENERATED_BODY()
    
public:    
    ACollectible();

protected:
    virtual void BeginPlay() override;

public:    
    virtual void Tick(float DeltaTime) override;
    
    // Called when collected by player
    UFUNCTION(BlueprintCallable)
    void Collect(class AAnimeRunnerCharacter* Player);
    
    // Get collectible properties
    UFUNCTION(BlueprintPure)
    ECollectibleType GetCollectibleType() const { return CollectibleType; }
    
    UFUNCTION(BlueprintPure)
    int32 GetValue() const { return Value; }

protected:
    // Components
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USphereComponent* CollisionSphere;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* MeshComponent;
    
    // Collectible properties
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectible")
    ECollectibleType CollectibleType;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectible")
    int32 Value;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectible")
    bool bRotates;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectible")
    float RotationSpeed;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectible")
    bool bFloats;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectible")
    float FloatAmplitude;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectible")
    float FloatSpeed;
    
    // Animation variables
    float InitialZ;
    float TimeAccumulator;
    
    // Collision handling
    UFUNCTION()
    void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, 
                        AActor* OtherActor, 
                        UPrimitiveComponent* OtherComp, 
                        int32 OtherBodyIndex, 
                        bool bFromSweep, 
                        const FHitResult& SweepResult);
};

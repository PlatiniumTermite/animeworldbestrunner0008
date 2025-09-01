#pragma once

#include "CoreMinimal.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "ObjectPool.generated.h"

UCLASS(Blueprintable)
class ANIMEWORLDRUNNER_API UObjectPool : public UObject
{
    GENERATED_BODY()

public:
    UObjectPool();
    
    // Initialize the object pool
    UFUNCTION(BlueprintCallable)
    void InitializePool(UClass* PooledObjectClass, int32 PoolSize, UWorld* World);
    
    // Get an object from the pool
    UFUNCTION(BlueprintCallable)
    AActor* GetPooledObject();
    
    // Return an object to the pool
    UFUNCTION(BlueprintCallable)
    void ReturnPooledObject(AActor* Actor);
    
    // Get pool stats
    UFUNCTION(BlueprintPure)
    int32 GetActiveCount() const { return ActiveObjects.Num(); }
    
    UFUNCTION(BlueprintPure)
    int32 GetInactiveCount() const { return InactiveObjects.Num(); }
    
    // Clear all objects in pool
    UFUNCTION(BlueprintCallable)
    void ClearPool();
    
    // Expand pool size
    UFUNCTION(BlueprintCallable)
    void ExpandPool(int32 AdditionalSize);

private:
    // Pooled object class
    UPROPERTY()
    UClass* PooledObjectClass;
    
    // Object pools
    UPROPERTY()
    TArray<AActor*> ActiveObjects;
    
    UPROPERTY()
    TArray<AActor*> InactiveObjects;
    
    // World reference
    UPROPERTY()
    UWorld* WorldContext;
    
    // Create a new pooled object
    AActor* CreatePooledObject();
};

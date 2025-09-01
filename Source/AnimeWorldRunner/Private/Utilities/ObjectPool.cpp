#include "Utilities/ObjectPool.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

UObjectPool::UObjectPool()
{
    PooledObjectClass = nullptr;
    WorldContext = nullptr;
}

void UObjectPool::InitializePool(UClass* InPooledObjectClass, int32 PoolSize, UWorld* World)
{
    if (!InPooledObjectClass || !World || PoolSize <= 0)
    {
        return;
    }
    
    PooledObjectClass = InPooledObjectClass;
    WorldContext = World;
    
    // Pre-allocate objects
    for (int32 i = 0; i < PoolSize; i++)
    {
        AActor* NewActor = CreatePooledObject();
        if (NewActor)
        {
            NewActor->SetActorHiddenInGame(true);
            NewActor->SetActorEnableCollision(false);
            InactiveObjects.Add(NewActor);
        }
    }
}

AActor* UObjectPool::GetPooledObject()
{
    if (!WorldContext)
    {
        return nullptr;
    }
    
    AActor* PooledActor = nullptr;
    
    // Try to get from inactive pool first
    if (InactiveObjects.Num() > 0)
    {
        PooledActor = InactiveObjects.Pop();
    }
    else
    {
        // Create new object if pool is empty
        PooledActor = CreatePooledObject();
    }
    
    if (PooledActor)
    {
        // Activate the object
        PooledActor->SetActorHiddenInGame(false);
        PooledActor->SetActorEnableCollision(true);
        ActiveObjects.Add(PooledActor);
    }
    
    return PooledActor;
}

void UObjectPool::ReturnPooledObject(AActor* Actor)
{
    if (!Actor)
    {
        return;
    }
    
    // Remove from active pool
    int32 Index = ActiveObjects.Find(Actor);
    if (Index != INDEX_NONE)
    {
        ActiveObjects.RemoveAt(Index);
        
        // Deactivate the object
        Actor->SetActorHiddenInGame(true);
        Actor->SetActorEnableCollision(false);
        Actor->SetActorLocation(FVector(0.0f, 0.0f, -10000.0f)); // Move far away
        
        // Add to inactive pool
        InactiveObjects.Add(Actor);
    }
}

void UObjectPool::ClearPool()
{
    // Destroy all objects
    for (AActor* Actor : ActiveObjects)
    {
        if (Actor)
        {
            Actor->Destroy();
        }
    }
    
    for (AActor* Actor : InactiveObjects)
    {
        if (Actor)
        {
            Actor->Destroy();
        }
    }
    
    ActiveObjects.Empty();
    InactiveObjects.Empty();
}

void UObjectPool::ExpandPool(int32 AdditionalSize)
{
    if (!PooledObjectClass || !WorldContext || AdditionalSize <= 0)
    {
        return;
    }
    
    for (int32 i = 0; i < AdditionalSize; i++)
    {
        AActor* NewActor = CreatePooledObject();
        if (NewActor)
        {
            NewActor->SetActorHiddenInGame(true);
            NewActor->SetActorEnableCollision(false);
            InactiveObjects.Add(NewActor);
        }
    }
}

AActor* UObjectPool::CreatePooledObject()
{
    if (!PooledObjectClass || !WorldContext)
    {
        return nullptr;
    }
    
    FVector SpawnLocation = FVector(0.0f, 0.0f, -10000.0f); // Spawn far away
    FRotator SpawnRotation = FRotator::ZeroRotator;
    
    return WorldContext->SpawnActor<AActor>(PooledObjectClass, SpawnLocation, SpawnRotation);
}

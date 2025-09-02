#include "Environment/ModularEnvironmentSystem.h"
#include "Materials/AnimeMaterialManager.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/StaticMesh.h"

AModularEnvironmentSystem::AModularEnvironmentSystem()
{
    PrimaryActorTick.bCanEverTick = true;
    
    // Initialize default settings
    ChunkSize = FVector(2000.0f, 2000.0f, 1000.0f);
    ChunkLoadRadius = 4000.0f;
    MaxLoadedChunks = 9; // 3x3 grid around player
    
    RandomSeed = 12345;
    PlatformDensity = 0.3f;
    VerticalVariation = 800.0f;
    FoliageDensity = 0.5f;
    
    // Performance settings
    bEnableInstancing = true;
    bEnableLOD = true;
    bEnableOcclusion = true;
    MaxDrawCalls = 80; // Mobile optimization
    
    // Create material manager
    MaterialManager = CreateDefaultSubobject<UAnimeMaterialManager>(TEXT("MaterialManager"));
    
    // Initialize timers
    LastPlayerLocation = FVector::ZeroVector;
    ChunkUpdateTimer = 0.0f;
}

void AModularEnvironmentSystem::BeginPlay()
{
    Super::BeginPlay();
    
    InitializeEnvironmentPieces();
    InitializeThemeSets();
    CreateInstancedMeshes();
    
    if (bEnableInstancing)
    {
        ApplyMobileOptimizations();
    }
}

void AModularEnvironmentSystem::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    ChunkUpdateTimer += DeltaTime;
    
    // Update chunks every 2 seconds to avoid performance issues
    if (ChunkUpdateTimer >= 2.0f)
    {
        // Get player location (this would typically come from the game mode or player controller)
        if (APawn* PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn())
        {
            UpdateEnvironmentAroundPlayer(PlayerPawn->GetActorLocation());
        }
        
        ChunkUpdateTimer = 0.0f;
    }
}

void AModularEnvironmentSystem::InitializeEnvironmentPieces()
{
    // Initialize default environment pieces
    // These would typically be loaded from data assets or configured in Blueprint
    
    FEnvironmentPieceData GroundPiece;
    GroundPiece.PieceType = EEnvironmentPieceType::Ground;
    GroundPiece.Scale = FVector(1.0f, 1.0f, 1.0f);
    GroundPiece.bCanBeInstanced = true;
    GroundPiece.SpawnWeight = 1.0f;
    GroundPiece.MaxInstances = 200;
    EnvironmentPieces.Add(EEnvironmentPieceType::Ground, GroundPiece);
    
    FEnvironmentPieceData PlatformPiece;
    PlatformPiece.PieceType = EEnvironmentPieceType::Platform;
    PlatformPiece.Scale = FVector(1.0f, 1.0f, 1.0f);
    PlatformPiece.bCanBeInstanced = true;
    PlatformPiece.SpawnWeight = 0.8f;
    PlatformPiece.MaxInstances = 150;
    EnvironmentPieces.Add(EEnvironmentPieceType::Platform, PlatformPiece);
    
    FEnvironmentPieceData TreePiece;
    TreePiece.PieceType = EEnvironmentPieceType::Tree;
    TreePiece.Scale = FVector(0.8f, 0.8f, 1.2f);
    TreePiece.bCanBeInstanced = true;
    TreePiece.SpawnWeight = 0.6f;
    TreePiece.MaxInstances = 300;
    EnvironmentPieces.Add(EEnvironmentPieceType::Tree, TreePiece);
    
    FEnvironmentPieceData RockPiece;
    RockPiece.PieceType = EEnvironmentPieceType::Rock;
    RockPiece.Scale = FVector(0.5f, 0.5f, 0.8f);
    RockPiece.bCanBeInstanced = true;
    RockPiece.SpawnWeight = 0.7f;
    RockPiece.MaxInstances = 250;
    EnvironmentPieces.Add(EEnvironmentPieceType::Rock, RockPiece);
}

void AModularEnvironmentSystem::InitializeThemeSets()
{
    // Forest theme
    TArray<EEnvironmentPieceType> ForestPieces;
    ForestPieces.Add(EEnvironmentPieceType::Ground);
    ForestPieces.Add(EEnvironmentPieceType::Platform);
    ForestPieces.Add(EEnvironmentPieceType::Tree);
    ForestPieces.Add(EEnvironmentPieceType::Foliage);
    ForestPieces.Add(EEnvironmentPieceType::Rock);
    ThemePieceSets.Add(EEnvironmentTheme::Forest, ForestPieces);
    
    // Mountain theme
    TArray<EEnvironmentPieceType> MountainPieces;
    MountainPieces.Add(EEnvironmentPieceType::Ground);
    MountainPieces.Add(EEnvironmentPieceType::Platform);
    MountainPieces.Add(EEnvironmentPieceType::Rock);
    MountainPieces.Add(EEnvironmentPieceType::Pillar);
    MountainPieces.Add(EEnvironmentPieceType::Stairs);
    ThemePieceSets.Add(EEnvironmentTheme::Mountain, MountainPieces);
    
    // Village theme
    TArray<EEnvironmentPieceType> VillagePieces;
    VillagePieces.Add(EEnvironmentPieceType::Ground);
    VillagePieces.Add(EEnvironmentPieceType::Building);
    VillagePieces.Add(EEnvironmentPieceType::Bridge);
    VillagePieces.Add(EEnvironmentPieceType::Decoration);
    ThemePieceSets.Add(EEnvironmentTheme::Village, VillagePieces);
}

void AModularEnvironmentSystem::CreateInstancedMeshes()
{
    if (!bEnableInstancing) return;
    
    // Create instanced mesh components for each piece type
    for (auto& PiecePair : EnvironmentPieces)
    {
        EEnvironmentPieceType PieceType = PiecePair.Key;
        FEnvironmentPieceData& PieceData = PiecePair.Value;
        
        if (PieceData.bCanBeInstanced && PieceData.Mesh)
        {
            FString ComponentName = FString::Printf(TEXT("InstancedMesh_%s"), *UEnum::GetValueAsString(PieceType));
            UInstancedStaticMeshComponent* InstancedComp = CreateDefaultSubobject<UInstancedStaticMeshComponent>(*ComponentName);
            
            InstancedComp->SetStaticMesh(PieceData.Mesh);
            InstancedComp->SetCollisionEnabled(PieceData.bEnableCollision ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
            InstancedComp->SetCastShadow(PieceData.bCastShadows);
            
            // Apply materials
            for (int32 i = 0; i < PieceData.Materials.Num(); i++)
            {
                if (PieceData.Materials[i])
                {
                    InstancedComp->SetMaterial(i, PieceData.Materials[i]);
                }
            }
            
            // Mobile optimizations
            if (bEnableLOD)
            {
                InstancedComp->bUseAsOccluder = bEnableOcclusion;
                InstancedComp->SetCullDistances(1500.0f, 3000.0f); // Start culling at 1.5km, fully cull at 3km
            }
            
            InstancedMeshComponents.Add(PieceType, InstancedComp);
        }
    }
}

void AModularEnvironmentSystem::GenerateEnvironmentChunk(FVector ChunkLocation, EEnvironmentTheme Theme, float DifficultyLevel)
{
    // Check if chunk already exists
    if (LoadedChunks.Contains(ChunkLocation))
    {
        return;
    }
    
    // Create new chunk data
    FEnvironmentChunkData NewChunk;
    NewChunk.ChunkLocation = ChunkLocation;
    NewChunk.ChunkSize = ChunkSize;
    NewChunk.Theme = Theme;
    NewChunk.DifficultyLevel = DifficultyLevel;
    NewChunk.bIsLoaded = false;
    
    // Generate procedural layout
    TArray<FTransform> GeneratedTransforms = GenerateProceduralLayout(ChunkLocation, Theme, DifficultyLevel);
    
    // Store the generated data
    for (int32 i = 0; i < GeneratedTransforms.Num(); i++)
    {
        NewChunk.PieceTransforms.Add(GeneratedTransforms[i]);
        NewChunk.PieceTypes.Add(SelectRandomPieceForTheme(Theme));
    }
    
    // Load the chunk
    LoadEnvironmentChunk(NewChunk);
    LoadedChunks.Add(ChunkLocation, NewChunk);
}

TArray<FTransform> AModularEnvironmentSystem::GenerateProceduralLayout(FVector ChunkLocation, EEnvironmentTheme Theme, float DifficultyLevel)
{
    TArray<FTransform> GeneratedTransforms;
    
    // Set random seed for consistent generation
    FMath::RandInit(RandomSeed + ChunkLocation.X + ChunkLocation.Y);
    
    // Generate base ground pieces
    int32 GroundPieceCount = FMath::RandRange(8, 15);
    for (int32 i = 0; i < GroundPieceCount; i++)
    {
        FVector RandomOffset = FVector(
            FMath::RandRange(-ChunkSize.X * 0.4f, ChunkSize.X * 0.4f),
            FMath::RandRange(-ChunkSize.Y * 0.4f, ChunkSize.Y * 0.4f),
            0.0f
        );
        
        FTransform GroundTransform;
        GroundTransform.SetLocation(ChunkLocation + RandomOffset);
        GroundTransform.SetRotation(FQuat::MakeFromEuler(FVector(0, 0, FMath::RandRange(0.0f, 360.0f))));
        GroundTransform.SetScale3D(FVector(1.0f + FMath::RandRange(-0.2f, 0.2f)));
        
        GeneratedTransforms.Add(GroundTransform);
    }
    
    // Generate platforms based on difficulty
    int32 PlatformCount = FMath::RoundToInt(PlatformDensity * DifficultyLevel * 10);
    for (int32 i = 0; i < PlatformCount; i++)
    {
        FVector RandomOffset = FVector(
            FMath::RandRange(-ChunkSize.X * 0.3f, ChunkSize.X * 0.3f),
            FMath::RandRange(-ChunkSize.Y * 0.3f, ChunkSize.Y * 0.3f),
            FMath::RandRange(100.0f, VerticalVariation * DifficultyLevel)
        );
        
        FTransform PlatformTransform;
        PlatformTransform.SetLocation(ChunkLocation + RandomOffset);
        PlatformTransform.SetRotation(FQuat::Identity);
        PlatformTransform.SetScale3D(FVector::OneVector);
        
        GeneratedTransforms.Add(PlatformTransform);
    }
    
    // Generate theme-specific decorations
    if (Theme == EEnvironmentTheme::Forest)
    {
        // Add trees and foliage
        int32 TreeCount = FMath::RoundToInt(FoliageDensity * 20);
        for (int32 i = 0; i < TreeCount; i++)
        {
            FVector TreeOffset = FVector(
                FMath::RandRange(-ChunkSize.X * 0.4f, ChunkSize.X * 0.4f),
                FMath::RandRange(-ChunkSize.Y * 0.4f, ChunkSize.Y * 0.4f),
                0.0f
            );
            
            FTransform TreeTransform;
            TreeTransform.SetLocation(ChunkLocation + TreeOffset);
            TreeTransform.SetRotation(FQuat::MakeFromEuler(FVector(0, 0, FMath::RandRange(0.0f, 360.0f))));
            TreeTransform.SetScale3D(FVector(FMath::RandRange(0.8f, 1.5f)));
            
            GeneratedTransforms.Add(TreeTransform);
        }
    }
    else if (Theme == EEnvironmentTheme::Mountain)
    {
        // Add rocks and vertical elements
        int32 RockCount = FMath::RandRange(5, 12);
        for (int32 i = 0; i < RockCount; i++)
        {
            FVector RockOffset = FVector(
                FMath::RandRange(-ChunkSize.X * 0.3f, ChunkSize.X * 0.3f),
                FMath::RandRange(-ChunkSize.Y * 0.3f, ChunkSize.Y * 0.3f),
                FMath::RandRange(0.0f, 200.0f)
            );
            
            FTransform RockTransform;
            RockTransform.SetLocation(ChunkLocation + RockOffset);
            RockTransform.SetRotation(FQuat::MakeFromEuler(FVector(
                FMath::RandRange(-15.0f, 15.0f),
                FMath::RandRange(-15.0f, 15.0f),
                FMath::RandRange(0.0f, 360.0f)
            )));
            RockTransform.SetScale3D(FVector(FMath::RandRange(0.5f, 2.0f)));
            
            GeneratedTransforms.Add(RockTransform);
        }
    }
    
    return GeneratedTransforms;
}

void AModularEnvironmentSystem::LoadEnvironmentChunk(const FEnvironmentChunkData& ChunkData)
{
    // Spawn all pieces in the chunk
    for (int32 i = 0; i < ChunkData.PieceTransforms.Num() && i < ChunkData.PieceTypes.Num(); i++)
    {
        EEnvironmentPieceType PieceType = ChunkData.PieceTypes[i];
        FTransform PieceTransform = ChunkData.PieceTransforms[i];
        
        SpawnEnvironmentPiece(PieceType, PieceTransform, bEnableInstancing);
    }
}

AActor* AModularEnvironmentSystem::SpawnEnvironmentPiece(EEnvironmentPieceType PieceType, FTransform SpawnTransform, bool bUseInstancing)
{
    FEnvironmentPieceData* PieceData = EnvironmentPieces.Find(PieceType);
    if (!PieceData || !PieceData->Mesh)
    {
        return nullptr;
    }
    
    if (bUseInstancing && PieceData->bCanBeInstanced)
    {
        // Use instanced mesh component
        if (UInstancedStaticMeshComponent** InstancedComp = InstancedMeshComponents.Find(PieceType))
        {
            if (*InstancedComp)
            {
                (*InstancedComp)->AddInstance(SpawnTransform);
            }
        }
        return nullptr; // No individual actor created for instanced meshes
    }
    else
    {
        // Create individual actor
        AActor* NewActor = GetWorld()->SpawnActor<AActor>();
        if (NewActor)
        {
            UStaticMeshComponent* MeshComp = NewObject<UStaticMeshComponent>(NewActor);
            MeshComp->SetStaticMesh(PieceData->Mesh);
            MeshComp->SetWorldTransform(SpawnTransform);
            MeshComp->SetCollisionEnabled(PieceData->bEnableCollision ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
            MeshComp->SetCastShadow(PieceData->bCastShadows);
            
            NewActor->SetRootComponent(MeshComp);
            
            // Apply materials
            for (int32 i = 0; i < PieceData->Materials.Num(); i++)
            {
                if (PieceData->Materials[i])
                {
                    MeshComp->SetMaterial(i, PieceData->Materials[i]);
                }
            }
        }
        return NewActor;
    }
}

void AModularEnvironmentSystem::UpdateEnvironmentAroundPlayer(FVector PlayerLocation, float LoadRadius)
{
    LastPlayerLocation = PlayerLocation;
    
    // Calculate which chunks should be loaded
    FVector PlayerChunkLocation = GetChunkLocationFromWorldLocation(PlayerLocation);
    
    // Load chunks in a grid around the player
    int32 ChunkRadius = FMath::CeilToInt(LoadRadius / ChunkSize.X);
    
    for (int32 X = -ChunkRadius; X <= ChunkRadius; X++)
    {
        for (int32 Y = -ChunkRadius; Y <= ChunkRadius; Y++)
        {
            FVector ChunkOffset = FVector(X * ChunkSize.X, Y * ChunkSize.Y, 0.0f);
            FVector ChunkLocation = PlayerChunkLocation + ChunkOffset;
            
            if (ShouldLoadChunk(ChunkLocation, PlayerLocation))
            {
                // Determine theme based on location (this could be more sophisticated)
                EEnvironmentTheme Theme = EEnvironmentTheme::Forest;
                if (FMath::Abs(ChunkLocation.X) > 4000.0f)
                {
                    Theme = EEnvironmentTheme::Mountain;
                }
                
                float DifficultyLevel = FMath::Clamp(FVector::Dist2D(ChunkLocation, FVector::ZeroVector) / 2000.0f, 1.0f, 3.0f);
                
                GenerateEnvironmentChunk(ChunkLocation, Theme, DifficultyLevel);
            }
        }
    }
    
    // Cleanup distant chunks
    CleanupDistantChunks(PlayerLocation);
}

FVector AModularEnvironmentSystem::GetChunkLocationFromWorldLocation(FVector WorldLocation)
{
    FVector ChunkLocation;
    ChunkLocation.X = FMath::FloorToInt(WorldLocation.X / ChunkSize.X) * ChunkSize.X;
    ChunkLocation.Y = FMath::FloorToInt(WorldLocation.Y / ChunkSize.Y) * ChunkSize.Y;
    ChunkLocation.Z = 0.0f; // Chunks are at ground level
    
    return ChunkLocation;
}

bool AModularEnvironmentSystem::ShouldLoadChunk(FVector ChunkLocation, FVector PlayerLocation)
{
    float Distance = FVector::Dist2D(ChunkLocation, PlayerLocation);
    return Distance <= ChunkLoadRadius && !LoadedChunks.Contains(ChunkLocation);
}

void AModularEnvironmentSystem::CleanupDistantChunks(FVector PlayerLocation)
{
    TArray<FVector> ChunksToRemove;
    
    for (auto& ChunkPair : LoadedChunks)
    {
        FVector ChunkLocation = ChunkPair.Key;
        float Distance = FVector::Dist2D(ChunkLocation, PlayerLocation);
        
        if (Distance > ChunkLoadRadius * 1.5f) // Unload chunks that are 1.5x the load radius
        {
            ChunksToRemove.Add(ChunkLocation);
        }
    }
    
    for (FVector ChunkLocation : ChunksToRemove)
    {
        UnloadEnvironmentChunk(ChunkLocation);
    }
}

void AModularEnvironmentSystem::UnloadEnvironmentChunk(FVector ChunkLocation)
{
    if (FEnvironmentChunkData* ChunkData = LoadedChunks.Find(ChunkLocation))
    {
        // For instanced meshes, we would need to track which instances belong to which chunk
        // This is a simplified version - in practice, you'd want to track instance indices
        
        LoadedChunks.Remove(ChunkLocation);
    }
}

EEnvironmentPieceType AModularEnvironmentSystem::SelectRandomPieceForTheme(EEnvironmentTheme Theme)
{
    if (TArray<EEnvironmentPieceType>* PieceTypes = ThemePieceSets.Find(Theme))
    {
        if (PieceTypes->Num() > 0)
        {
            int32 RandomIndex = FMath::RandRange(0, PieceTypes->Num() - 1);
            return (*PieceTypes)[RandomIndex];
        }
    }
    
    return EEnvironmentPieceType::Ground; // Default fallback
}

void AModularEnvironmentSystem::ApplyMobileOptimizations()
{
    // Reduce instance counts for mobile
    for (auto& PiecePair : EnvironmentPieces)
    {
        FEnvironmentPieceData& PieceData = PiecePair.Value;
        PieceData.MaxInstances = FMath::RoundToInt(PieceData.MaxInstances * 0.7f); // Reduce by 30%
    }
    
    // Adjust LOD distances for mobile
    SetLODDistances(800.0f, 1500.0f, 2500.0f);
}

void AModularEnvironmentSystem::SetLODDistances(float LOD1Distance, float LOD2Distance, float CullDistance)
{
    for (auto& ComponentPair : InstancedMeshComponents)
    {
        UInstancedStaticMeshComponent* InstancedComp = ComponentPair.Value;
        if (InstancedComp)
        {
            InstancedComp->SetCullDistances(LOD2Distance, CullDistance);
        }
    }
}

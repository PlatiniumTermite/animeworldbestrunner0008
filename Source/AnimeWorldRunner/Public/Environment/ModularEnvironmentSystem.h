#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Materials/AnimeMaterialManager.h"
#include "ModularEnvironmentSystem.generated.h"

UENUM(BlueprintType)
enum class EEnvironmentPieceType : uint8
{
    Ground          UMETA(DisplayName = "Ground Piece"),
    Platform        UMETA(DisplayName = "Platform"),
    Wall            UMETA(DisplayName = "Wall"),
    Pillar          UMETA(DisplayName = "Pillar"),
    Stairs          UMETA(DisplayName = "Stairs"),
    Bridge          UMETA(DisplayName = "Bridge"),
    Tree            UMETA(DisplayName = "Tree"),
    Rock            UMETA(DisplayName = "Rock"),
    Foliage         UMETA(DisplayName = "Foliage"),
    Water           UMETA(DisplayName = "Water"),
    Building        UMETA(DisplayName = "Building"),
    Decoration      UMETA(DisplayName = "Decoration")
};

UENUM(BlueprintType)
enum class EEnvironmentTheme : uint8
{
    Forest          UMETA(DisplayName = "Forest"),
    Mountain        UMETA(DisplayName = "Mountain"),
    Beach           UMETA(DisplayName = "Beach"),
    Village         UMETA(DisplayName = "Village"),
    Ruins           UMETA(DisplayName = "Ancient Ruins"),
    Sky             UMETA(DisplayName = "Sky Islands"),
    Cave            UMETA(DisplayName = "Cave"),
    Desert          UMETA(DisplayName = "Desert")
};

USTRUCT(BlueprintType)
struct FEnvironmentPieceData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Piece Data")
    EEnvironmentPieceType PieceType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Piece Data")
    UStaticMesh* Mesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Piece Data")
    TArray<UMaterialInterface*> Materials;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Piece Data")
    FVector Scale;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Piece Data")
    bool bCanBeInstanced;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Piece Data")
    float SpawnWeight;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Piece Data")
    int32 MaxInstances;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Piece Data")
    bool bEnableCollision;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Piece Data")
    bool bCastShadows;

    FEnvironmentPieceData()
    {
        PieceType = EEnvironmentPieceType::Ground;
        Mesh = nullptr;
        Scale = FVector::OneVector;
        bCanBeInstanced = false;
        SpawnWeight = 1.0f;
        MaxInstances = 100;
        bEnableCollision = true;
        bCastShadows = true;
    }
};

USTRUCT(BlueprintType)
struct FEnvironmentChunkData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chunk")
    FVector ChunkLocation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chunk")
    FVector ChunkSize;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chunk")
    EEnvironmentTheme Theme;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chunk")
    TArray<FTransform> PieceTransforms;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chunk")
    TArray<EEnvironmentPieceType> PieceTypes;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chunk")
    bool bIsLoaded;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chunk")
    float DifficultyLevel;

    FEnvironmentChunkData()
    {
        ChunkLocation = FVector::ZeroVector;
        ChunkSize = FVector(2000.0f, 2000.0f, 1000.0f);
        Theme = EEnvironmentTheme::Forest;
        bIsLoaded = false;
        DifficultyLevel = 1.0f;
    }
};

UCLASS()
class ANIMEWORLDRUNNER_API AModularEnvironmentSystem : public AActor
{
    GENERATED_BODY()

public:
    AModularEnvironmentSystem();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    // Environment generation
    UFUNCTION(BlueprintCallable, Category = "Environment")
    void GenerateEnvironmentChunk(FVector ChunkLocation, EEnvironmentTheme Theme, float DifficultyLevel = 1.0f);

    UFUNCTION(BlueprintCallable, Category = "Environment")
    void LoadEnvironmentChunk(const FEnvironmentChunkData& ChunkData);

    UFUNCTION(BlueprintCallable, Category = "Environment")
    void UnloadEnvironmentChunk(FVector ChunkLocation);

    UFUNCTION(BlueprintCallable, Category = "Environment")
    void UpdateEnvironmentAroundPlayer(FVector PlayerLocation, float LoadRadius = 3000.0f);

    // Procedural generation
    UFUNCTION(BlueprintCallable, Category = "Procedural")
    TArray<FTransform> GenerateProceduralLayout(FVector ChunkLocation, EEnvironmentTheme Theme, float DifficultyLevel);

    UFUNCTION(BlueprintCallable, Category = "Procedural")
    void PlacePlatforms(FVector StartLocation, FVector EndLocation, float PlatformSpacing = 500.0f);

    UFUNCTION(BlueprintCallable, Category = "Procedural")
    void CreateVerticalSection(FVector BaseLocation, float Height, int32 Levels);

    UFUNCTION(BlueprintCallable, Category = "Procedural")
    void SpawnFoliage(FVector ChunkLocation, FVector ChunkSize, int32 FoliageCount);

    // Environment pieces management
    UFUNCTION(BlueprintCallable, Category = "Environment Pieces")
    void RegisterEnvironmentPiece(EEnvironmentPieceType PieceType, const FEnvironmentPieceData& PieceData);

    UFUNCTION(BlueprintCallable, Category = "Environment Pieces")
    AActor* SpawnEnvironmentPiece(EEnvironmentPieceType PieceType, FTransform SpawnTransform, bool bUseInstancing = true);

    UFUNCTION(BlueprintCallable, Category = "Environment Pieces")
    void CreateInstancedMeshes();

    // Material system integration
    UFUNCTION(BlueprintCallable, Category = "Materials")
    void ApplyThemeMaterials(EEnvironmentTheme Theme);

    UFUNCTION(BlueprintCallable, Category = "Materials")
    void UpdateEnvironmentLighting(FVector SunDirection, float TimeOfDay);

    // Optimization
    UFUNCTION(BlueprintCallable, Category = "Optimization")
    void OptimizeForMobile();

    UFUNCTION(BlueprintCallable, Category = "Optimization")
    void SetLODDistances(float LOD1Distance, float LOD2Distance, float CullDistance);

protected:
    // Environment piece registry
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment Data")
    TMap<EEnvironmentPieceType, FEnvironmentPieceData> EnvironmentPieces;

    // Theme-based piece sets
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Themes")
    TMap<EEnvironmentTheme, TArray<EEnvironmentPieceType>> ThemePieceSets;

    // Instanced mesh components for optimization
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Instancing")
    TMap<EEnvironmentPieceType, UInstancedStaticMeshComponent*> InstancedMeshComponents;

    // Chunk management
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Chunks")
    TMap<FVector, FEnvironmentChunkData> LoadedChunks;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chunks")
    FVector ChunkSize;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chunks")
    float ChunkLoadRadius;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chunks")
    int32 MaxLoadedChunks;

    // Material manager
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Materials")
    UAnimeMaterialManager* MaterialManager;

    // Generation settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation")
    int32 RandomSeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation")
    float PlatformDensity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation")
    float VerticalVariation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation")
    float FoliageDensity;

    // Performance settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Performance")
    bool bEnableInstancing;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Performance")
    bool bEnableLOD;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Performance")
    bool bEnableOcclusion;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Performance")
    int32 MaxDrawCalls;

private:
    // Helper functions
    void InitializeEnvironmentPieces();
    void InitializeThemeSets();
    FVector GetChunkLocationFromWorldLocation(FVector WorldLocation);
    bool ShouldLoadChunk(FVector ChunkLocation, FVector PlayerLocation);
    void CleanupDistantChunks(FVector PlayerLocation);
    EEnvironmentPieceType SelectRandomPieceForTheme(EEnvironmentTheme Theme);
    FTransform GenerateRandomTransform(FVector BaseLocation, EEnvironmentPieceType PieceType);
    void ApplyMobileOptimizations();

    // Current player location for chunk streaming
    FVector LastPlayerLocation;
    float ChunkUpdateTimer;
};

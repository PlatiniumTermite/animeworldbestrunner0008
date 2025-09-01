#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AWRGameModeBase.generated.h"

UENUM(BlueprintType)
enum class EGameState : uint8
{
    MENU,
    PLAYING,
    PAUSED,
    GAME_OVER
};

UCLASS()
class ANIMEWORLDRUNNER_API AAWRGameModeBase : public AGameModeBase
{
    GENERATED_BODY()

public:
    AAWRGameModeBase();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;
    
    // Game state management
    UFUNCTION(BlueprintCallable)
    void StartGame();
    
    UFUNCTION(BlueprintCallable)
    void PauseGame();
    
    UFUNCTION(BlueprintCallable)
    void ResumeGame();
    
    UFUNCTION(BlueprintCallable)
    void EndGame();
    
    UFUNCTION(BlueprintCallable)
    void RestartGame();
    
    // Score management
    UFUNCTION(BlueprintCallable)
    void AddScore(int32 Points);
    
    UFUNCTION(BlueprintPure)
    int32 GetCurrentScore() const { return CurrentScore; }
    
    UFUNCTION(BlueprintPure)
    int32 GetHighScore() const { return HighScore; }
    
    // Distance tracking
    UFUNCTION(BlueprintPure)
    float GetDistanceTraveled() const { return DistanceTraveled; }
    
    // Game state checks
    UFUNCTION(BlueprintPure)
    bool IsGameRunning() const { return CurrentGameState == EGameState::PLAYING; }
    
    UFUNCTION(BlueprintPure)
    bool IsGamePaused() const { return CurrentGameState == EGameState::PAUSED; }
    
    UFUNCTION(BlueprintPure)
    EGameState GetCurrentGameState() const { return CurrentGameState; }

    // Environment management
    UFUNCTION(BlueprintCallable)
    void SpawnEnvironmentPiece();
    
    UFUNCTION(BlueprintCallable)
    void CleanupOldEnvironment();

private:
    // Game state
    UPROPERTY(VisibleAnywhere, Category = "Game State")
    EGameState CurrentGameState;
    
    // Scoring
    UPROPERTY(VisibleAnywhere, Category = "Scoring")
    int32 CurrentScore;
    
    UPROPERTY(VisibleAnywhere, Category = "Scoring")
    int32 HighScore;
    
    UPROPERTY(VisibleAnywhere, Category = "Scoring")
    float DistanceTraveled;
    
    UPROPERTY(VisibleAnywhere, Category = "Scoring")
    float GameTime;
    
    // Reference to player character
    UPROPERTY()
    class AAnimeRunnerCharacter* PlayerCharacter;
    
    // Environment spawning
    UPROPERTY(EditAnywhere, Category = "Environment")
    float EnvironmentSpawnDistance;
    
    UPROPERTY(EditAnywhere, Category = "Environment")
    float EnvironmentCleanupDistance;
    
    UPROPERTY(EditAnywhere, Category = "Environment")
    TArray<TSubclassOf<AActor>> EnvironmentPieces;
    
    UPROPERTY()
    TArray<AActor*> SpawnedEnvironment;
    
    // Last spawn position
    float LastSpawnPosition;
    
    // Save/Load game data
    void LoadGameData();
    void SaveGameData();
    
    // Update distance based on player movement
    void UpdateDistanceTraveled(float DeltaTime);
};

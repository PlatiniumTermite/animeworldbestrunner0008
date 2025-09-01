#include "GameModes/AWRGameModeBase.h"
#include "AnimeRunnerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "GameFramework/SaveGame.h"

AAWRGameModeBase::AAWRGameModeBase()
{
    PrimaryActorTick.bCanEverTick = true;
    
    // Set default pawn class to our custom character
    DefaultPawnClass = AAnimeRunnerCharacter::StaticClass();
    
    // Initialize game state
    CurrentGameState = EGameState::MENU;
    CurrentScore = 0;
    HighScore = 0;
    DistanceTraveled = 0.0f;
    GameTime = 0.0f;
    
    // Environment settings
    EnvironmentSpawnDistance = 2000.0f;
    EnvironmentCleanupDistance = -1000.0f;
    LastSpawnPosition = 0.0f;
}

void AAWRGameModeBase::BeginPlay()
{
    Super::BeginPlay();
    
    // Get reference to player character
    PlayerCharacter = Cast<AAnimeRunnerCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
    
    // Load saved game data
    LoadGameData();
}

void AAWRGameModeBase::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    if (CurrentGameState == EGameState::PLAYING)
    {
        GameTime += DeltaTime;
        UpdateDistanceTraveled(DeltaTime);
        
        // Spawn environment pieces as needed
        if (PlayerCharacter)
        {
            float PlayerX = PlayerCharacter->GetActorLocation().X;
            if (PlayerX + EnvironmentSpawnDistance > LastSpawnPosition)
            {
                SpawnEnvironmentPiece();
            }
        }
        
        // Cleanup old environment
        CleanupOldEnvironment();
    }
}

void AAWRGameModeBase::StartGame()
{
    CurrentGameState = EGameState::PLAYING;
    CurrentScore = 0;
    DistanceTraveled = 0.0f;
    GameTime = 0.0f;
    
    // Reset player character state
    if (PlayerCharacter)
    {
        PlayerCharacter->SetActorLocation(FVector(0.0f, 0.0f, 100.0f));
    }
}

void AAWRGameModeBase::PauseGame()
{
    if (CurrentGameState == EGameState::PLAYING)
    {
        CurrentGameState = EGameState::PAUSED;
        UGameplayStatics::SetGamePaused(this, true);
    }
}

void AAWRGameModeBase::ResumeGame()
{
    if (CurrentGameState == EGameState::PAUSED)
    {
        CurrentGameState = EGameState::PLAYING;
        UGameplayStatics::SetGamePaused(this, false);
    }
}

void AAWRGameModeBase::EndGame()
{
    CurrentGameState = EGameState::GAME_OVER;
    
    // Update high score if needed
    if (CurrentScore > HighScore)
    {
        HighScore = CurrentScore;
        SaveGameData();
    }
}

void AAWRGameModeBase::RestartGame()
{
    // Clean up existing environment
    for (AActor* Actor : SpawnedEnvironment)
    {
        if (Actor)
        {
            Actor->Destroy();
        }
    }
    SpawnedEnvironment.Empty();
    LastSpawnPosition = 0.0f;
    
    // Restart the game
    StartGame();
}

void AAWRGameModeBase::AddScore(int32 Points)
{
    if (Points > 0 && CurrentGameState == EGameState::PLAYING)
    {
        CurrentScore += Points;
    }
}

void AAWRGameModeBase::SpawnEnvironmentPiece()
{
    if (EnvironmentPieces.Num() > 0)
    {
        // Choose random environment piece
        int32 RandomIndex = FMath::RandRange(0, EnvironmentPieces.Num() - 1);
        TSubclassOf<AActor> PieceClass = EnvironmentPieces[RandomIndex];
        
        if (PieceClass)
        {
            FVector SpawnLocation = FVector(LastSpawnPosition + 1000.0f, 0.0f, 0.0f);
            FRotator SpawnRotation = FRotator::ZeroRotator;
            
            AActor* SpawnedPiece = GetWorld()->SpawnActor<AActor>(PieceClass, SpawnLocation, SpawnRotation);
            if (SpawnedPiece)
            {
                SpawnedEnvironment.Add(SpawnedPiece);
                LastSpawnPosition = SpawnLocation.X;
            }
        }
    }
}

void AAWRGameModeBase::CleanupOldEnvironment()
{
    if (PlayerCharacter)
    {
        float PlayerX = PlayerCharacter->GetActorLocation().X;
        
        for (int32 i = SpawnedEnvironment.Num() - 1; i >= 0; i--)
        {
            AActor* Actor = SpawnedEnvironment[i];
            if (Actor && Actor->GetActorLocation().X < PlayerX + EnvironmentCleanupDistance)
            {
                Actor->Destroy();
                SpawnedEnvironment.RemoveAt(i);
            }
        }
    }
}

void AAWRGameModeBase::UpdateDistanceTraveled(float DeltaTime)
{
    if (PlayerCharacter)
    {
        float Speed = PlayerCharacter->GetCurrentSpeed();
        DistanceTraveled += Speed * DeltaTime * 0.01f; // Convert to meters
        
        // Add score based on distance
        int32 DistanceScore = FMath::FloorToInt(DistanceTraveled * 0.1f);
        if (DistanceScore > CurrentScore)
        {
            AddScore(DistanceScore - CurrentScore);
        }
    }
}

void AAWRGameModeBase::LoadGameData()
{
    // TODO: Implement save game loading
    // For now, just set a default high score
    HighScore = 0;
}

void AAWRGameModeBase::SaveGameData()
{
    // TODO: Implement save game saving
    // This would save the high score and other persistent data
}

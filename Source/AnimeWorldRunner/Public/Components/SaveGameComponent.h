#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/SaveGame.h"
#include "SaveGameComponent.generated.h"

USTRUCT(BlueprintType)
struct FPlayerSaveData
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 HighScore;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 TotalCoins;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float BestDistance;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 GamesPlayed;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> UnlockedCharacters;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> UnlockedPowerUps;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString CurrentCharacter;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bSoundEnabled;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bMusicEnabled;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float SoundVolume;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MusicVolume;

    FPlayerSaveData()
    {
        HighScore = 0;
        TotalCoins = 0;
        BestDistance = 0.0f;
        GamesPlayed = 0;
        CurrentCharacter = TEXT("Default");
        bSoundEnabled = true;
        bMusicEnabled = true;
        SoundVolume = 1.0f;
        MusicVolume = 0.8f;
    }
};

UCLASS(Blueprintable)
class ANIMEWORLDRUNNER_API UAnimeWorldRunnerSaveGame : public USaveGame
{
    GENERATED_BODY()

public:
    UAnimeWorldRunnerSaveGame();

    UPROPERTY(VisibleAnywhere, Category = "Save Data")
    FPlayerSaveData PlayerData;
    
    UPROPERTY(VisibleAnywhere, Category = "Save Data")
    FString SaveSlotName;
    
    UPROPERTY(VisibleAnywhere, Category = "Save Data")
    uint32 UserIndex;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ANIMEWORLDRUNNER_API USaveGameComponent : public UActorComponent
{
    GENERATED_BODY()

public:    
    USaveGameComponent();

protected:
    virtual void BeginPlay() override;

public:    
    // Save/Load functions
    UFUNCTION(BlueprintCallable)
    bool SaveGame();
    
    UFUNCTION(BlueprintCallable)
    bool LoadGame();
    
    UFUNCTION(BlueprintCallable)
    void ResetSaveData();
    
    // Data access functions
    UFUNCTION(BlueprintPure)
    int32 GetHighScore() const { return SaveData.PlayerData.HighScore; }
    
    UFUNCTION(BlueprintCallable)
    void SetHighScore(int32 NewHighScore);
    
    UFUNCTION(BlueprintPure)
    int32 GetTotalCoins() const { return SaveData.PlayerData.TotalCoins; }
    
    UFUNCTION(BlueprintCallable)
    void AddCoins(int32 Amount);
    
    UFUNCTION(BlueprintCallable)
    bool SpendCoins(int32 Amount);
    
    UFUNCTION(BlueprintPure)
    float GetBestDistance() const { return SaveData.PlayerData.BestDistance; }
    
    UFUNCTION(BlueprintCallable)
    void SetBestDistance(float NewDistance);
    
    UFUNCTION(BlueprintPure)
    int32 GetGamesPlayed() const { return SaveData.PlayerData.GamesPlayed; }
    
    UFUNCTION(BlueprintCallable)
    void IncrementGamesPlayed();
    
    // Character management
    UFUNCTION(BlueprintCallable)
    void UnlockCharacter(const FString& CharacterName);
    
    UFUNCTION(BlueprintPure)
    bool IsCharacterUnlocked(const FString& CharacterName) const;
    
    UFUNCTION(BlueprintCallable)
    void SetCurrentCharacter(const FString& CharacterName);
    
    UFUNCTION(BlueprintPure)
    FString GetCurrentCharacter() const { return SaveData.PlayerData.CurrentCharacter; }
    
    // Power-up management
    UFUNCTION(BlueprintCallable)
    void UnlockPowerUp(const FString& PowerUpName);
    
    UFUNCTION(BlueprintPure)
    bool IsPowerUpUnlocked(const FString& PowerUpName) const;
    
    // Settings
    UFUNCTION(BlueprintCallable)
    void SetSoundEnabled(bool bEnabled);
    
    UFUNCTION(BlueprintPure)
    bool IsSoundEnabled() const { return SaveData.PlayerData.bSoundEnabled; }
    
    UFUNCTION(BlueprintCallable)
    void SetMusicEnabled(bool bEnabled);
    
    UFUNCTION(BlueprintPure)
    bool IsMusicEnabled() const { return SaveData.PlayerData.bMusicEnabled; }
    
    UFUNCTION(BlueprintCallable)
    void SetSoundVolume(float Volume);
    
    UFUNCTION(BlueprintPure)
    float GetSoundVolume() const { return SaveData.PlayerData.SoundVolume; }
    
    UFUNCTION(BlueprintCallable)
    void SetMusicVolume(float Volume);
    
    UFUNCTION(BlueprintPure)
    float GetMusicVolume() const { return SaveData.PlayerData.MusicVolume; }
    
    // Get full save data
    UFUNCTION(BlueprintPure)
    const FPlayerSaveData& GetPlayerData() const { return SaveData.PlayerData; }

private:
    // Save data instance
    UPROPERTY()
    UAnimeWorldRunnerSaveGame* SaveData;
    
    // Save slot name
    UPROPERTY(EditAnywhere, Category = "Save System")
    FString SaveSlotName;
    
    // User index
    UPROPERTY(EditAnywhere, Category = "Save System")
    uint32 UserIndex;
    
    // Create new save data
    void CreateNewSaveData();
};

#include "Components/SaveGameComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"

UAnimeWorldRunnerSaveGame::UAnimeWorldRunnerSaveGame()
{
    SaveSlotName = TEXT("AnimeWorldRunnerSave");
    UserIndex = 0;
}

USaveGameComponent::USaveGameComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    
    SaveSlotName = TEXT("AnimeWorldRunnerSave");
    UserIndex = 0;
    SaveData = nullptr;
}

void USaveGameComponent::BeginPlay()
{
    Super::BeginPlay();
    
    // Try to load existing save data, create new if none exists
    if (!LoadGame())
    {
        CreateNewSaveData();
    }
}

bool USaveGameComponent::SaveGame()
{
    if (!SaveData)
    {
        return false;
    }
    
    return UGameplayStatics::SaveGameToSlot(SaveData, SaveSlotName, UserIndex);
}

bool USaveGameComponent::LoadGame()
{
    if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, UserIndex))
    {
        SaveData = Cast<UAnimeWorldRunnerSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, UserIndex));
        return SaveData != nullptr;
    }
    
    return false;
}

void USaveGameComponent::ResetSaveData()
{
    CreateNewSaveData();
    SaveGame();
}

void USaveGameComponent::SetHighScore(int32 NewHighScore)
{
    if (SaveData && NewHighScore > SaveData->PlayerData.HighScore)
    {
        SaveData->PlayerData.HighScore = NewHighScore;
        SaveGame();
    }
}

void USaveGameComponent::AddCoins(int32 Amount)
{
    if (SaveData && Amount > 0)
    {
        SaveData->PlayerData.TotalCoins += Amount;
        SaveGame();
    }
}

bool USaveGameComponent::SpendCoins(int32 Amount)
{
    if (SaveData && Amount > 0 && SaveData->PlayerData.TotalCoins >= Amount)
    {
        SaveData->PlayerData.TotalCoins -= Amount;
        SaveGame();
        return true;
    }
    return false;
}

void USaveGameComponent::SetBestDistance(float NewDistance)
{
    if (SaveData && NewDistance > SaveData->PlayerData.BestDistance)
    {
        SaveData->PlayerData.BestDistance = NewDistance;
        SaveGame();
    }
}

void USaveGameComponent::IncrementGamesPlayed()
{
    if (SaveData)
    {
        SaveData->PlayerData.GamesPlayed++;
        SaveGame();
    }
}

void USaveGameComponent::UnlockCharacter(const FString& CharacterName)
{
    if (SaveData && !IsCharacterUnlocked(CharacterName))
    {
        SaveData->PlayerData.UnlockedCharacters.Add(CharacterName);
        SaveGame();
    }
}

bool USaveGameComponent::IsCharacterUnlocked(const FString& CharacterName) const
{
    if (SaveData)
    {
        return SaveData->PlayerData.UnlockedCharacters.Contains(CharacterName);
    }
    return false;
}

void USaveGameComponent::SetCurrentCharacter(const FString& CharacterName)
{
    if (SaveData && IsCharacterUnlocked(CharacterName))
    {
        SaveData->PlayerData.CurrentCharacter = CharacterName;
        SaveGame();
    }
}

void USaveGameComponent::UnlockPowerUp(const FString& PowerUpName)
{
    if (SaveData && !IsPowerUpUnlocked(PowerUpName))
    {
        SaveData->PlayerData.UnlockedPowerUps.Add(PowerUpName);
        SaveGame();
    }
}

bool USaveGameComponent::IsPowerUpUnlocked(const FString& PowerUpName) const
{
    if (SaveData)
    {
        return SaveData->PlayerData.UnlockedPowerUps.Contains(PowerUpName);
    }
    return false;
}

void USaveGameComponent::SetSoundEnabled(bool bEnabled)
{
    if (SaveData)
    {
        SaveData->PlayerData.bSoundEnabled = bEnabled;
        SaveGame();
    }
}

void USaveGameComponent::SetMusicEnabled(bool bEnabled)
{
    if (SaveData)
    {
        SaveData->PlayerData.bMusicEnabled = bEnabled;
        SaveGame();
    }
}

void USaveGameComponent::SetSoundVolume(float Volume)
{
    if (SaveData)
    {
        SaveData->PlayerData.SoundVolume = FMath::Clamp(Volume, 0.0f, 1.0f);
        SaveGame();
    }
}

void USaveGameComponent::SetMusicVolume(float Volume)
{
    if (SaveData)
    {
        SaveData->PlayerData.MusicVolume = FMath::Clamp(Volume, 0.0f, 1.0f);
        SaveGame();
    }
}

void USaveGameComponent::CreateNewSaveData()
{
    SaveData = Cast<UAnimeWorldRunnerSaveGame>(UGameplayStatics::CreateSaveGameObject(UAnimeWorldRunnerSaveGame::StaticClass()));
    
    if (SaveData)
    {
        SaveData->SaveSlotName = SaveSlotName;
        SaveData->UserIndex = UserIndex;
        
        // Initialize with default character unlocked
        SaveData->PlayerData.UnlockedCharacters.Add(TEXT("Default"));
        SaveData->PlayerData.CurrentCharacter = TEXT("Default");
    }
}

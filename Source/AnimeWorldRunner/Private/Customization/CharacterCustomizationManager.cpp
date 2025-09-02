#include "Customization/CharacterCustomizationManager.h"
#include "AnimeRunnerCharacter.h"
#include "Engine/DataTable.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "GameFramework/SaveGame.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystem.h"

UCharacterCustomizationManager::UCharacterCustomizationManager()
{
    PrimaryComponentTick.bCanEverTick = false;
    
    bIsPreviewActive = false;
    TargetCharacter = nullptr;
    CustomizationItemsTable = nullptr;
    CharacterPresetsTable = nullptr;
    CustomizationSaveGameClass = nullptr;
}

void UCharacterCustomizationManager::BeginPlay()
{
    Super::BeginPlay();
    
    InitializeCustomization();
    LoadCustomization();
}

void UCharacterCustomizationManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UCharacterCustomizationManager::InitializeCustomization()
{
    // Get reference to the character
    TargetCharacter = Cast<AAnimeRunnerCharacter>(GetOwner());
    if (!TargetCharacter)
    {
        // Try to find the character in the world
        if (APawn* PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn())
        {
            TargetCharacter = Cast<AAnimeRunnerCharacter>(PlayerPawn);
        }
    }
    
    if (TargetCharacter)
    {
        SetupCharacterReferences();
        CreateDynamicMaterials();
    }
    
    // Initialize default customization if none exists
    if (CurrentCustomization.EquippedItems.Num() == 0)
    {
        CurrentCustomization = FPlayerCustomization();
    }
}

void UCharacterCustomizationManager::SetupCharacterReferences()
{
    if (!TargetCharacter) return;
    
    // Get mesh components from character
    if (USkeletalMeshComponent* CharacterMesh = TargetCharacter->GetMesh())
    {
        CharacterMeshComponents.Add(ECustomizationCategory::Hair, CharacterMesh);
        CharacterMeshComponents.Add(ECustomizationCategory::Eyes, CharacterMesh);
        CharacterMeshComponents.Add(ECustomizationCategory::Skin, CharacterMesh);
        CharacterMeshComponents.Add(ECustomizationCategory::Outfit, CharacterMesh);
    }
    
    // Look for additional mesh components (accessories, weapons, etc.)
    TArray<UMeshComponent*> MeshComponents;
    TargetCharacter->GetComponents<UMeshComponent>(MeshComponents);
    
    for (UMeshComponent* MeshComp : MeshComponents)
    {
        FString ComponentName = MeshComp->GetName();
        
        if (ComponentName.Contains("Accessory"))
        {
            CharacterMeshComponents.Add(ECustomizationCategory::Accessories, MeshComp);
        }
        else if (ComponentName.Contains("Weapon"))
        {
            CharacterMeshComponents.Add(ECustomizationCategory::Weapon, MeshComp);
        }
    }
}

void UCharacterCustomizationManager::CreateDynamicMaterials()
{
    for (auto& MeshPair : CharacterMeshComponents)
    {
        ECustomizationCategory Category = MeshPair.Key;
        UMeshComponent* MeshComponent = MeshPair.Value;
        
        if (MeshComponent && MeshComponent->GetMaterial(0))
        {
            UMaterialInstanceDynamic* DynMaterial = MeshComponent->CreateDynamicMaterialInstance(0);
            if (DynMaterial)
            {
                DynamicMaterials.Add(Category, DynMaterial);
            }
        }
    }
}

void UCharacterCustomizationManager::EquipItem(ECustomizationCategory Category, const FString& ItemID)
{
    if (!IsItemUnlocked(ItemID))
    {
        return;
    }
    
    // Update current customization
    CurrentCustomization.EquippedItems.Add(Category, ItemID);
    
    // Update visual appearance
    UpdateMeshComponent(Category, ItemID);
    
    // Play equip effect
    FCustomizationItem ItemData = GetItemData(ItemID);
    PlayEquipEffect(ItemData);
    
    // Notify change
    NotifyCustomizationChange(Category, ItemID);
    
    // Auto-save
    SaveCustomization();
}

void UCharacterCustomizationManager::SetItemColor(ECustomizationCategory Category, FLinearColor Color)
{
    CurrentCustomization.ItemColors.Add(Category, Color);
    
    UpdateMaterialColor(Category, Color);
    
    OnColorChanged.Broadcast(Category, Color);
    
    SaveCustomization();
}

FString UCharacterCustomizationManager::GetEquippedItem(ECustomizationCategory Category)
{
    if (FString* ItemID = CurrentCustomization.EquippedItems.Find(Category))
    {
        return *ItemID;
    }
    return "";
}

FLinearColor UCharacterCustomizationManager::GetItemColor(ECustomizationCategory Category)
{
    if (FLinearColor* Color = CurrentCustomization.ItemColors.Find(Category))
    {
        return *Color;
    }
    return FLinearColor::White;
}

void UCharacterCustomizationManager::ApplyPreset(const FString& PresetID)
{
    if (!IsPresetUnlocked(PresetID)) return;
    
    FCharacterPreset* PresetData = nullptr;
    if (CharacterPresetsTable)
    {
        PresetData = CharacterPresetsTable->FindRow<FCharacterPreset>(*PresetID, TEXT(""));
    }
    
    if (PresetData)
    {
        // Apply all items from preset
        for (auto& ItemPair : PresetData->EquippedItems)
        {
            ECustomizationCategory Category = ItemPair.Key;
            FString ItemID = ItemPair.Value;
            
            if (IsItemUnlocked(ItemID))
            {
                CurrentCustomization.EquippedItems.Add(Category, ItemID);
                UpdateMeshComponent(Category, ItemID);
            }
        }
        
        // Apply colors
        for (auto& ColorPair : PresetData->ItemColors)
        {
            ECustomizationCategory Category = ColorPair.Key;
            FLinearColor Color = ColorPair.Value;
            
            CurrentCustomization.ItemColors.Add(Category, Color);
            UpdateMaterialColor(Category, Color);
        }
        
        UpdateCharacterAppearance();
        SaveCustomization();
    }
}

void UCharacterCustomizationManager::SaveCurrentAsPreset(const FString& PresetID, const FText& PresetName)
{
    // This would typically save to a user presets data table or save file
    // Implementation depends on your save system architecture
}

TArray<FCustomizationItem> UCharacterCustomizationManager::GetItemsByCategory(ECustomizationCategory Category)
{
    TArray<FCustomizationItem> CategoryItems;
    
    if (CustomizationItemsTable)
    {
        TArray<FCustomizationItem*> AllItems;
        CustomizationItemsTable->GetAllRows<FCustomizationItem>(TEXT(""), AllItems);
        
        for (FCustomizationItem* Item : AllItems)
        {
            if (Item && Item->Category == Category)
            {
                CategoryItems.Add(*Item);
            }
        }
    }
    
    return CategoryItems;
}

FCustomizationItem UCharacterCustomizationManager::GetItemData(const FString& ItemID)
{
    if (CustomizationItemsTable)
    {
        FCustomizationItem* ItemData = CustomizationItemsTable->FindRow<FCustomizationItem>(*ItemID, TEXT(""));
        if (ItemData)
        {
            return *ItemData;
        }
    }
    
    return FCustomizationItem(); // Return default item
}

bool UCharacterCustomizationManager::IsItemUnlocked(const FString& ItemID)
{
    // Check if item is in unlocked list
    return CurrentCustomization.UnlockedItems.Contains(ItemID) || 
           GetItemData(ItemID).UnlockType == EUnlockType::Default;
}

bool UCharacterCustomizationManager::CanUnlockItem(const FString& ItemID)
{
    if (IsItemUnlocked(ItemID)) return true;
    
    FCustomizationItem ItemData = GetItemData(ItemID);
    return CheckUnlockRequirements(ItemData);
}

bool UCharacterCustomizationManager::UnlockItem(const FString& ItemID)
{
    if (IsItemUnlocked(ItemID)) return true;
    
    FCustomizationItem ItemData = GetItemData(ItemID);
    
    if (!CheckUnlockRequirements(ItemData)) return false;
    
    // Process unlock based on type
    switch (ItemData.UnlockType)
    {
        case EUnlockType::Coins:
            if (!SpendCoins(ItemData.UnlockCost)) return false;
            break;
        case EUnlockType::Premium:
            // Handle premium currency (not implemented in this example)
            break;
        case EUnlockType::Achievement:
        case EUnlockType::Level:
        case EUnlockType::Event:
            // These are checked in CheckUnlockRequirements
            break;
    }
    
    // Add to unlocked items
    CurrentCustomization.UnlockedItems.AddUnique(ItemID);
    
    // Broadcast unlock event
    OnItemUnlocked.Broadcast(ItemID);
    
    // Save progress
    SaveCustomization();
    
    return true;
}

TArray<FCharacterPreset> UCharacterCustomizationManager::GetAvailablePresets()
{
    TArray<FCharacterPreset> AvailablePresets;
    
    if (CharacterPresetsTable)
    {
        TArray<FCharacterPreset*> AllPresets;
        CharacterPresetsTable->GetAllRows<FCharacterPreset>(TEXT(""), AllPresets);
        
        for (FCharacterPreset* Preset : AllPresets)
        {
            if (Preset)
            {
                AvailablePresets.Add(*Preset);
            }
        }
    }
    
    return AvailablePresets;
}

bool UCharacterCustomizationManager::IsPresetUnlocked(const FString& PresetID)
{
    return CurrentCustomization.UnlockedPresets.Contains(PresetID);
}

bool UCharacterCustomizationManager::UnlockPreset(const FString& PresetID)
{
    if (IsPresetUnlocked(PresetID)) return true;
    
    // Add unlock logic here based on preset requirements
    CurrentCustomization.UnlockedPresets.AddUnique(PresetID);
    SaveCustomization();
    
    return true;
}

void UCharacterCustomizationManager::UpdateCharacterAppearance()
{
    for (auto& ItemPair : CurrentCustomization.EquippedItems)
    {
        ECustomizationCategory Category = ItemPair.Key;
        FString ItemID = ItemPair.Value;
        
        UpdateMeshComponent(Category, ItemID);
    }
    
    for (auto& ColorPair : CurrentCustomization.ItemColors)
    {
        ECustomizationCategory Category = ColorPair.Key;
        FLinearColor Color = ColorPair.Value;
        
        UpdateMaterialColor(Category, Color);
    }
}

void UCharacterCustomizationManager::PreviewItem(ECustomizationCategory Category, const FString& ItemID, FLinearColor Color)
{
    if (!bIsPreviewActive)
    {
        PreviewCustomization = CurrentCustomization;
        bIsPreviewActive = true;
    }
    
    // Apply preview changes
    UpdateMeshComponent(Category, ItemID);
    UpdateMaterialColor(Category, Color);
}

void UCharacterCustomizationManager::CancelPreview()
{
    if (bIsPreviewActive)
    {
        // Restore original appearance
        for (auto& ItemPair : CurrentCustomization.EquippedItems)
        {
            ECustomizationCategory Category = ItemPair.Key;
            FString ItemID = ItemPair.Value;
            UpdateMeshComponent(Category, ItemID);
        }
        
        for (auto& ColorPair : CurrentCustomization.ItemColors)
        {
            ECustomizationCategory Category = ColorPair.Key;
            FLinearColor Color = ColorPair.Value;
            UpdateMaterialColor(Category, Color);
        }
        
        bIsPreviewActive = false;
    }
}

void UCharacterCustomizationManager::SaveCustomization()
{
    // This would save to a save game object
    // Implementation depends on your save system
    if (CustomizationSaveGameClass)
    {
        // Create save game instance and save customization data
        // UGameplayStatics::SaveGameToSlot(SaveGameInstance, "CustomizationSave", 0);
    }
}

void UCharacterCustomizationManager::LoadCustomization()
{
    // Load from save game
    if (CustomizationSaveGameClass)
    {
        // Load customization data from save game
        // if (USaveGame* LoadedGame = UGameplayStatics::LoadGameFromSlot("CustomizationSave", 0))
        // {
        //     // Apply loaded customization
        // }
    }
    
    // Apply loaded or default customization
    UpdateCharacterAppearance();
}

void UCharacterCustomizationManager::ResetToDefaults()
{
    CurrentCustomization = FPlayerCustomization();
    UpdateCharacterAppearance();
    SaveCustomization();
}

int32 UCharacterCustomizationManager::GetPlayerCoins()
{
    // This would get coins from game instance or save data
    // Placeholder implementation
    return 1000;
}

bool UCharacterCustomizationManager::SpendCoins(int32 Amount)
{
    int32 CurrentCoins = GetPlayerCoins();
    if (CurrentCoins >= Amount)
    {
        // Deduct coins (implementation depends on your currency system)
        return true;
    }
    return false;
}

int32 UCharacterCustomizationManager::GetPlayerLevel()
{
    // Get player level from game instance or save data
    return 1;
}

bool UCharacterCustomizationManager::HasAchievement(const FString& AchievementID)
{
    // Check achievement system
    return false;
}

void UCharacterCustomizationManager::UpdateMeshComponent(ECustomizationCategory Category, const FString& ItemID)
{
    FCustomizationItem ItemData = GetItemData(ItemID);
    
    if (UMeshComponent** MeshComp = CharacterMeshComponents.Find(Category))
    {
        if (*MeshComp)
        {
            // Update skeletal mesh
            if (USkeletalMeshComponent* SkelMeshComp = Cast<USkeletalMeshComponent>(*MeshComp))
            {
                if (ItemData.SkeletalMesh)
                {
                    SkelMeshComp->SetSkeletalMesh(ItemData.SkeletalMesh);
                }
            }
            // Update static mesh
            else if (UStaticMeshComponent* StaticMeshComp = Cast<UStaticMeshComponent>(*MeshComp))
            {
                if (ItemData.Mesh)
                {
                    StaticMeshComp->SetStaticMesh(ItemData.Mesh);
                }
            }
            
            // Apply materials
            for (int32 i = 0; i < ItemData.Materials.Num(); i++)
            {
                if (ItemData.Materials[i])
                {
                    (*MeshComp)->SetMaterial(i, ItemData.Materials[i]);
                }
            }
        }
    }
}

void UCharacterCustomizationManager::UpdateMaterialColor(ECustomizationCategory Category, FLinearColor Color)
{
    if (UMaterialInstanceDynamic** DynMaterial = DynamicMaterials.Find(Category))
    {
        if (*DynMaterial)
        {
            // Set color parameters based on category
            switch (Category)
            {
                case ECustomizationCategory::Hair:
                    (*DynMaterial)->SetVectorParameterValue("HairColor", Color);
                    break;
                case ECustomizationCategory::Eyes:
                    (*DynMaterial)->SetVectorParameterValue("EyeColor", Color);
                    break;
                case ECustomizationCategory::Skin:
                    (*DynMaterial)->SetVectorParameterValue("SkinColor", Color);
                    break;
                case ECustomizationCategory::Outfit:
                    (*DynMaterial)->SetVectorParameterValue("OutfitColor", Color);
                    break;
                case ECustomizationCategory::Accessories:
                    (*DynMaterial)->SetVectorParameterValue("AccessoryColor", Color);
                    break;
                default:
                    (*DynMaterial)->SetVectorParameterValue("BaseColor", Color);
                    break;
            }
        }
    }
}

void UCharacterCustomizationManager::PlayEquipEffect(const FCustomizationItem& Item)
{
    // Play equip sound
    if (Item.EquipSound)
    {
        UGameplayStatics::PlaySound2D(GetWorld(), Item.EquipSound);
    }
    
    // Play special effect
    if (Item.bHasSpecialEffect && Item.SpecialEffect && TargetCharacter)
    {
        UGameplayStatics::SpawnEmitterAttached(
            Item.SpecialEffect,
            TargetCharacter->GetMesh(),
            NAME_None,
            FVector::ZeroVector,
            FRotator::ZeroRotator,
            EAttachLocation::KeepWorldPosition,
            true
        );
    }
}

bool UCharacterCustomizationManager::CheckUnlockRequirements(const FCustomizationItem& Item)
{
    switch (Item.UnlockType)
    {
        case EUnlockType::Default:
            return true;
            
        case EUnlockType::Coins:
            return GetPlayerCoins() >= Item.UnlockCost;
            
        case EUnlockType::Level:
            return GetPlayerLevel() >= Item.RequiredLevel;
            
        case EUnlockType::Achievement:
            return HasAchievement(Item.RequiredAchievement);
            
        case EUnlockType::Premium:
            // Check premium currency
            return false; // Not implemented
            
        case EUnlockType::Event:
            // Check event availability
            return false; // Not implemented
    }
    
    return false;
}

void UCharacterCustomizationManager::NotifyCustomizationChange(ECustomizationCategory Category, const FString& ItemID)
{
    OnCustomizationChanged.Broadcast(Category, ItemID);
}

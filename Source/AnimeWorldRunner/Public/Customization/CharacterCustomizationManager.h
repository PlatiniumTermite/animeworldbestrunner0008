#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "Materials/MaterialInterface.h"
#include "CharacterCustomizationManager.generated.h"

UENUM(BlueprintType)
enum class ECustomizationCategory : uint8
{
    Hair            UMETA(DisplayName = "Hair"),
    Eyes            UMETA(DisplayName = "Eyes"),
    Skin            UMETA(DisplayName = "Skin"),
    Outfit          UMETA(DisplayName = "Outfit"),
    Accessories     UMETA(DisplayName = "Accessories"),
    Weapon          UMETA(DisplayName = "Weapon"),
    Aura            UMETA(DisplayName = "Aura Effect"),
    Voice           UMETA(DisplayName = "Voice")
};

UENUM(BlueprintType)
enum class EUnlockType : uint8
{
    Default         UMETA(DisplayName = "Default"),
    Coins           UMETA(DisplayName = "Purchase with Coins"),
    Achievement     UMETA(DisplayName = "Achievement Unlock"),
    Premium         UMETA(DisplayName = "Premium Currency"),
    Event           UMETA(DisplayName = "Special Event"),
    Level           UMETA(DisplayName = "Level Requirement")
};

USTRUCT(BlueprintType)
struct FCustomizationItem : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Info")
    FString ItemID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Info")
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Info")
    FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Info")
    ECustomizationCategory Category;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
    class UStaticMesh* Mesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
    class USkeletalMesh* SkeletalMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
    TArray<class UMaterialInterface*> Materials;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
    class UTexture2D* PreviewIcon;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Colors")
    TArray<FLinearColor> AvailableColors;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Colors")
    FLinearColor DefaultColor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unlock")
    EUnlockType UnlockType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unlock")
    int32 UnlockCost;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unlock")
    int32 RequiredLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unlock")
    FString RequiredAchievement;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rarity")
    int32 RarityLevel; // 1-5 stars

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
    bool bHasSpecialEffect;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
    class UParticleSystem* SpecialEffect;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    class USoundCue* EquipSound;

    FCustomizationItem()
    {
        ItemID = "";
        DisplayName = FText::FromString("Default Item");
        Description = FText::FromString("Default customization item");
        Category = ECustomizationCategory::Hair;
        Mesh = nullptr;
        SkeletalMesh = nullptr;
        PreviewIcon = nullptr;
        DefaultColor = FLinearColor::White;
        UnlockType = EUnlockType::Default;
        UnlockCost = 0;
        RequiredLevel = 1;
        RequiredAchievement = "";
        RarityLevel = 1;
        bHasSpecialEffect = false;
        SpecialEffect = nullptr;
        EquipSound = nullptr;
    }
};

USTRUCT(BlueprintType)
struct FCharacterPreset : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Preset")
    FString PresetID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Preset")
    FText PresetName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Preset")
    class UTexture2D* PresetIcon;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Items")
    TMap<ECustomizationCategory, FString> EquippedItems;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Colors")
    TMap<ECustomizationCategory, FLinearColor> ItemColors;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unlock")
    EUnlockType UnlockType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unlock")
    int32 UnlockCost;

    FCharacterPreset()
    {
        PresetID = "";
        PresetName = FText::FromString("Default Preset");
        PresetIcon = nullptr;
        UnlockType = EUnlockType::Default;
        UnlockCost = 0;
    }
};

USTRUCT(BlueprintType)
struct FPlayerCustomization
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customization")
    TMap<ECustomizationCategory, FString> EquippedItems;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Customization")
    TMap<ECustomizationCategory, FLinearColor> ItemColors;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unlocks")
    TArray<FString> UnlockedItems;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Unlocks")
    TArray<FString> UnlockedPresets;

    FPlayerCustomization()
    {
        // Initialize with default items
        EquippedItems.Add(ECustomizationCategory::Hair, "default_hair");
        EquippedItems.Add(ECustomizationCategory::Eyes, "default_eyes");
        EquippedItems.Add(ECustomizationCategory::Skin, "default_skin");
        EquippedItems.Add(ECustomizationCategory::Outfit, "default_outfit");
        
        // Default colors
        ItemColors.Add(ECustomizationCategory::Hair, FLinearColor(0.3f, 0.2f, 0.1f, 1.0f)); // Brown
        ItemColors.Add(ECustomizationCategory::Eyes, FLinearColor(0.2f, 0.4f, 0.8f, 1.0f)); // Blue
        ItemColors.Add(ECustomizationCategory::Skin, FLinearColor(0.9f, 0.7f, 0.6f, 1.0f)); // Fair
        ItemColors.Add(ECustomizationCategory::Outfit, FLinearColor::White);
    }
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCustomizationChanged, ECustomizationCategory, Category, const FString&, ItemID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnColorChanged, ECustomizationCategory, Category, FLinearColor, NewColor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemUnlocked, const FString&, ItemID);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ANIMEWORLDRUNNER_API UCharacterCustomizationManager : public UActorComponent
{
    GENERATED_BODY()

public:
    UCharacterCustomizationManager();

protected:
    virtual void BeginPlay() override;

public:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // Customization Management
    UFUNCTION(BlueprintCallable, Category = "Character Customization")
    void EquipItem(ECustomizationCategory Category, const FString& ItemID);

    UFUNCTION(BlueprintCallable, Category = "Character Customization")
    void SetItemColor(ECustomizationCategory Category, FLinearColor Color);

    UFUNCTION(BlueprintCallable, Category = "Character Customization")
    FString GetEquippedItem(ECustomizationCategory Category);

    UFUNCTION(BlueprintCallable, Category = "Character Customization")
    FLinearColor GetItemColor(ECustomizationCategory Category);

    UFUNCTION(BlueprintCallable, Category = "Character Customization")
    void ApplyPreset(const FString& PresetID);

    UFUNCTION(BlueprintCallable, Category = "Character Customization")
    void SaveCurrentAsPreset(const FString& PresetID, const FText& PresetName);

    // Item Management
    UFUNCTION(BlueprintCallable, Category = "Character Customization")
    TArray<FCustomizationItem> GetItemsByCategory(ECustomizationCategory Category);

    UFUNCTION(BlueprintCallable, Category = "Character Customization")
    FCustomizationItem GetItemData(const FString& ItemID);

    UFUNCTION(BlueprintCallable, Category = "Character Customization")
    bool IsItemUnlocked(const FString& ItemID);

    UFUNCTION(BlueprintCallable, Category = "Character Customization")
    bool CanUnlockItem(const FString& ItemID);

    UFUNCTION(BlueprintCallable, Category = "Character Customization")
    bool UnlockItem(const FString& ItemID);

    // Preset Management
    UFUNCTION(BlueprintCallable, Category = "Character Customization")
    TArray<FCharacterPreset> GetAvailablePresets();

    UFUNCTION(BlueprintCallable, Category = "Character Customization")
    bool IsPresetUnlocked(const FString& PresetID);

    UFUNCTION(BlueprintCallable, Category = "Character Customization")
    bool UnlockPreset(const FString& PresetID);

    // Visual Updates
    UFUNCTION(BlueprintCallable, Category = "Character Customization")
    void UpdateCharacterAppearance();

    UFUNCTION(BlueprintCallable, Category = "Character Customization")
    void PreviewItem(ECustomizationCategory Category, const FString& ItemID, FLinearColor Color = FLinearColor::White);

    UFUNCTION(BlueprintCallable, Category = "Character Customization")
    void CancelPreview();

    // Save/Load System
    UFUNCTION(BlueprintCallable, Category = "Character Customization")
    void SaveCustomization();

    UFUNCTION(BlueprintCallable, Category = "Character Customization")
    void LoadCustomization();

    UFUNCTION(BlueprintCallable, Category = "Character Customization")
    void ResetToDefaults();

    // Currency and Unlocks
    UFUNCTION(BlueprintCallable, Category = "Character Customization")
    int32 GetPlayerCoins();

    UFUNCTION(BlueprintCallable, Category = "Character Customization")
    bool SpendCoins(int32 Amount);

    UFUNCTION(BlueprintCallable, Category = "Character Customization")
    int32 GetPlayerLevel();

    UFUNCTION(BlueprintCallable, Category = "Character Customization")
    bool HasAchievement(const FString& AchievementID);

    // Events
    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnCustomizationChanged OnCustomizationChanged;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnColorChanged OnColorChanged;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnItemUnlocked OnItemUnlocked;

protected:
    // Data Tables
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
    class UDataTable* CustomizationItemsTable;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
    class UDataTable* CharacterPresetsTable;

    // Current Customization
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Current State")
    FPlayerCustomization CurrentCustomization;

    // Preview State
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Preview")
    bool bIsPreviewActive;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Preview")
    FPlayerCustomization PreviewCustomization;

    // Character References
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character")
    class AAnimeRunnerCharacter* TargetCharacter;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character")
    TMap<ECustomizationCategory, class UMeshComponent*> CharacterMeshComponents;

    // Material Instances for customization
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Materials")
    TMap<ECustomizationCategory, class UMaterialInstanceDynamic*> DynamicMaterials;

    // Save Game
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save System")
    TSubclassOf<class USaveGame> CustomizationSaveGameClass;

private:
    // Helper functions
    void InitializeCustomization();
    void SetupCharacterReferences();
    void CreateDynamicMaterials();
    void UpdateMeshComponent(ECustomizationCategory Category, const FString& ItemID);
    void UpdateMaterialColor(ECustomizationCategory Category, FLinearColor Color);
    void PlayEquipEffect(const FCustomizationItem& Item);
    bool CheckUnlockRequirements(const FCustomizationItem& Item);
    void NotifyCustomizationChange(ECustomizationCategory Category, const FString& ItemID);
};

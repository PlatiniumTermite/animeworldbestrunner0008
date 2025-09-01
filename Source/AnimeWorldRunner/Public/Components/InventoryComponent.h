#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/Texture2D.h"
#include "InventoryComponent.generated.h"

UENUM(BlueprintType)
enum class EPowerUpType : uint8
{
    NONE,
    MAGNET,
    SHIELD,
    BOOST,
    JETPACK
};

USTRUCT(BlueprintType)
struct FInventoryItem
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName ItemID;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Quantity;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTexture2D* Icon;

    FInventoryItem()
    {
        ItemID = NAME_None;
        Quantity = 0;
        Icon = nullptr;
    }
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ANIMEWORLDRUNNER_API UInventoryComponent : public UActorComponent
{
    GENERATED_BODY()

public:    
    UInventoryComponent();

protected:
    virtual void BeginPlay() override;

public:    
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    
    // Currency functions
    UFUNCTION(BlueprintCallable)
    void AddCoins(int32 Amount);
    
    UFUNCTION(BlueprintCallable)
    bool SpendCoins(int32 Amount);
    
    UFUNCTION(BlueprintPure)
    int32 GetCoinCount() const { return Coins; }
    
    // Power-up functions
    UFUNCTION(BlueprintCallable)
    void AddPowerUp(EPowerUpType Type, int32 Duration);
    
    UFUNCTION(BlueprintCallable)
    void UsePowerUp();
    
    UFUNCTION(BlueprintPure)
    bool HasPowerUp() const { return ActivePowerUp != EPowerUpType::NONE; }
    
    UFUNCTION(BlueprintPure)
    EPowerUpType GetActivePowerUp() const { return ActivePowerUp; }
    
    UFUNCTION(BlueprintPure)
    float GetPowerUpTimeRemaining() const { return PowerUpTimeRemaining; }
    
    // Item functions
    UFUNCTION(BlueprintCallable)
    void AddItem(const FInventoryItem& Item);
    
    UFUNCTION(BlueprintCallable)
    bool RemoveItem(const FName& ItemID, int32 Quantity = 1);
    
    UFUNCTION(BlueprintPure)
    const TArray<FInventoryItem>& GetItems() const { return Items; }

    // Score functions
    UFUNCTION(BlueprintCallable)
    void AddScore(int32 Points);
    
    UFUNCTION(BlueprintPure)
    int32 GetCurrentScore() const { return CurrentScore; }

private:
    // Currency
    UPROPERTY(VisibleAnywhere, Category = "Inventory")
    int32 Coins;
    
    // Score
    UPROPERTY(VisibleAnywhere, Category = "Inventory")
    int32 CurrentScore;
    
    // Active power-up
    UPROPERTY(VisibleAnywhere, Category = "Power-ups")
    EPowerUpType ActivePowerUp;
    
    UPROPERTY(VisibleAnywhere, Category = "Power-ups")
    float PowerUpTimeRemaining;
    
    // Inventory items
    UPROPERTY(VisibleAnywhere, Category = "Inventory")
    TArray<FInventoryItem> Items;
    
    // Handle power-up timer
    FTimerHandle PowerUpTimerHandle;
    
    // Clear active power-up
    UFUNCTION()
    void ClearPowerUp();
};

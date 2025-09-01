#include "Components/InventoryComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"

UInventoryComponent::UInventoryComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    
    // Initialize values
    Coins = 0;
    CurrentScore = 0;
    ActivePowerUp = EPowerUpType::NONE;
    PowerUpTimeRemaining = 0.0f;
}

void UInventoryComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    
    // Update power-up timer
    if (ActivePowerUp != EPowerUpType::NONE && PowerUpTimeRemaining > 0.0f)
    {
        PowerUpTimeRemaining -= DeltaTime;
        if (PowerUpTimeRemaining <= 0.0f)
        {
            ClearPowerUp();
        }
    }
}

void UInventoryComponent::AddCoins(int32 Amount)
{
    if (Amount > 0)
    {
        Coins += Amount;
        AddScore(Amount); // Also add to score
    }
}

bool UInventoryComponent::SpendCoins(int32 Amount)
{
    if (Amount > 0 && Coins >= Amount)
    {
        Coins -= Amount;
        return true;
    }
    return false;
}

void UInventoryComponent::AddPowerUp(EPowerUpType Type, int32 Duration)
{
    if (Type != EPowerUpType::NONE)
    {
        ActivePowerUp = Type;
        PowerUpTimeRemaining = Duration;
        
        // Clear any existing timer
        if (GetWorld())
        {
            GetWorld()->GetTimerManager().ClearTimer(PowerUpTimerHandle);
        }
    }
}

void UInventoryComponent::UsePowerUp()
{
    if (ActivePowerUp != EPowerUpType::NONE)
    {
        // Implement power-up effects based on type
        switch (ActivePowerUp)
        {
            case EPowerUpType::MAGNET:
                // Attract nearby coins
                break;
            case EPowerUpType::SHIELD:
                // Provide invincibility
                break;
            case EPowerUpType::BOOST:
                // Increase speed temporarily
                break;
            case EPowerUpType::JETPACK:
                // Allow extended jumping/flying
                break;
            default:
                break;
        }
    }
}

void UInventoryComponent::AddItem(const FInventoryItem& Item)
{
    // Check if item already exists
    for (FInventoryItem& ExistingItem : Items)
    {
        if (ExistingItem.ItemID == Item.ItemID)
        {
            ExistingItem.Quantity += Item.Quantity;
            return;
        }
    }
    
    // Add new item
    Items.Add(Item);
}

bool UInventoryComponent::RemoveItem(const FName& ItemID, int32 Quantity)
{
    for (int32 i = 0; i < Items.Num(); i++)
    {
        if (Items[i].ItemID == ItemID)
        {
            if (Items[i].Quantity >= Quantity)
            {
                Items[i].Quantity -= Quantity;
                if (Items[i].Quantity <= 0)
                {
                    Items.RemoveAt(i);
                }
                return true;
            }
            break;
        }
    }
    return false;
}

void UInventoryComponent::AddScore(int32 Points)
{
    if (Points > 0)
    {
        CurrentScore += Points;
    }
}

void UInventoryComponent::ClearPowerUp()
{
    ActivePowerUp = EPowerUpType::NONE;
    PowerUpTimeRemaining = 0.0f;
    
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(PowerUpTimerHandle);
    }
}

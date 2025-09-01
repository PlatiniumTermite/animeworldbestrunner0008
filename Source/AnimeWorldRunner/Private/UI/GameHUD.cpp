#include "UI/GameHUD.h"
#include "Blueprint/UserWidget.h"
#include "Components/InventoryComponent.h"
#include "GameModes/AWRGameModeBase.h"
#include "Kismet/GameplayStatics.h"

AGameHUD::AGameHUD()
{
    PrimaryActorTick.bCanEverTick = false;
    
    // Initialize widget pointers
    MainMenuWidget = nullptr;
    GameUIWidget = nullptr;
    PauseMenuWidget = nullptr;
    GameOverWidget = nullptr;
    CurrentWidget = nullptr;
}

void AGameHUD::BeginPlay()
{
    Super::BeginPlay();
    
    CreateWidgets();
    ShowMainMenu();
}

void AGameHUD::DrawHUD()
{
    Super::DrawHUD();
    
    // Custom HUD drawing can be added here if needed
}

void AGameHUD::ShowMainMenu()
{
    SwitchToWidget(MainMenuWidget);
}

void AGameHUD::ShowGameUI()
{
    SwitchToWidget(GameUIWidget);
}

void AGameHUD::ShowPauseMenu()
{
    SwitchToWidget(PauseMenuWidget);
}

void AGameHUD::ShowGameOverMenu()
{
    SwitchToWidget(GameOverWidget);
}

void AGameHUD::HideAllUI()
{
    SwitchToWidget(nullptr);
}

void AGameHUD::UpdateScore(int32 NewScore)
{
    // Update score in game UI widget
    if (GameUIWidget)
    {
        // Call Blueprint function to update score display
        // This would be implemented in the Blueprint version of the widget
    }
}

void AGameHUD::UpdateCoins(int32 NewCoins)
{
    // Update coins in game UI widget
    if (GameUIWidget)
    {
        // Call Blueprint function to update coins display
    }
}

void AGameHUD::UpdateDistance(float NewDistance)
{
    // Update distance in game UI widget
    if (GameUIWidget)
    {
        // Call Blueprint function to update distance display
    }
}

void AGameHUD::UpdatePowerUp(EPowerUpType PowerUpType, float TimeRemaining)
{
    // Update power-up display in game UI widget
    if (GameUIWidget)
    {
        // Call Blueprint function to update power-up display
    }
}

void AGameHUD::CreateWidgets()
{
    if (!GetWorld())
    {
        return;
    }
    
    // Create main menu widget
    if (MainMenuWidgetClass && !MainMenuWidget)
    {
        MainMenuWidget = CreateWidget<UUserWidget>(GetWorld(), MainMenuWidgetClass);
    }
    
    // Create game UI widget
    if (GameUIWidgetClass && !GameUIWidget)
    {
        GameUIWidget = CreateWidget<UUserWidget>(GetWorld(), GameUIWidgetClass);
    }
    
    // Create pause menu widget
    if (PauseMenuWidgetClass && !PauseMenuWidget)
    {
        PauseMenuWidget = CreateWidget<UUserWidget>(GetWorld(), PauseMenuWidgetClass);
    }
    
    // Create game over widget
    if (GameOverWidgetClass && !GameOverWidget)
    {
        GameOverWidget = CreateWidget<UUserWidget>(GetWorld(), GameOverWidgetClass);
    }
}

void AGameHUD::SwitchToWidget(UUserWidget* NewWidget)
{
    // Remove current widget from viewport
    if (CurrentWidget)
    {
        CurrentWidget->RemoveFromViewport();
    }
    
    // Add new widget to viewport
    if (NewWidget)
    {
        NewWidget->AddToViewport();
    }
    
    CurrentWidget = NewWidget;
}

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Blueprint/UserWidget.h"
#include "GameHUD.generated.h"

UCLASS()
class ANIMEWORLDRUNNER_API AGameHUD : public AHUD
{
    GENERATED_BODY()

public:
    AGameHUD();

protected:
    virtual void BeginPlay() override;

public:
    virtual void DrawHUD() override;
    
    // UI Management
    UFUNCTION(BlueprintCallable)
    void ShowMainMenu();
    
    UFUNCTION(BlueprintCallable)
    void ShowGameUI();
    
    UFUNCTION(BlueprintCallable)
    void ShowPauseMenu();
    
    UFUNCTION(BlueprintCallable)
    void ShowGameOverMenu();
    
    UFUNCTION(BlueprintCallable)
    void HideAllUI();
    
    // Update UI elements
    UFUNCTION(BlueprintCallable)
    void UpdateScore(int32 NewScore);
    
    UFUNCTION(BlueprintCallable)
    void UpdateCoins(int32 NewCoins);
    
    UFUNCTION(BlueprintCallable)
    void UpdateDistance(float NewDistance);
    
    UFUNCTION(BlueprintCallable)
    void UpdatePowerUp(class EPowerUpType PowerUpType, float TimeRemaining);

protected:
    // Widget classes
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> MainMenuWidgetClass;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> GameUIWidgetClass;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> PauseMenuWidgetClass;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> GameOverWidgetClass;
    
    // Widget instances
    UPROPERTY()
    UUserWidget* MainMenuWidget;
    
    UPROPERTY()
    UUserWidget* GameUIWidget;
    
    UPROPERTY()
    UUserWidget* PauseMenuWidget;
    
    UPROPERTY()
    UUserWidget* GameOverWidget;
    
    // Current active widget
    UPROPERTY()
    UUserWidget* CurrentWidget;
    
    // Create widget instances
    void CreateWidgets();
    
    // Switch between widgets
    void SwitchToWidget(UUserWidget* NewWidget);
};

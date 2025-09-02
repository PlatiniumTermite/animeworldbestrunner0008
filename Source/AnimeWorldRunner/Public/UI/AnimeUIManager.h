#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/Widget.h"
#include "Animation/UMGSequencePlayer.h"
#include "AnimeUIManager.generated.h"

UENUM(BlueprintType)
enum class EAnimeUIType : uint8
{
    MainMenu        UMETA(DisplayName = "Main Menu"),
    GameHUD         UMETA(DisplayName = "Game HUD"),
    PauseMenu       UMETA(DisplayName = "Pause Menu"),
    GameOver        UMETA(DisplayName = "Game Over"),
    Settings        UMETA(DisplayName = "Settings"),
    Inventory       UMETA(DisplayName = "Inventory"),
    CharacterMenu   UMETA(DisplayName = "Character Menu"),
    Shop            UMETA(DisplayName = "Shop"),
    Dialogue        UMETA(DisplayName = "Dialogue"),
    Notification    UMETA(DisplayName = "Notification")
};

UENUM(BlueprintType)
enum class EAnimeUIAnimation : uint8
{
    FadeIn          UMETA(DisplayName = "Fade In"),
    FadeOut         UMETA(DisplayName = "Fade Out"),
    SlideIn         UMETA(DisplayName = "Slide In"),
    SlideOut        UMETA(DisplayName = "Slide Out"),
    ScaleIn         UMETA(DisplayName = "Scale In"),
    ScaleOut        UMETA(DisplayName = "Scale Out"),
    Bounce          UMETA(DisplayName = "Bounce"),
    Shake           UMETA(DisplayName = "Shake"),
    Glow            UMETA(DisplayName = "Glow Effect"),
    Sparkle         UMETA(DisplayName = "Sparkle Effect")
};

USTRUCT(BlueprintType)
struct FAnimeUIStyle
{
    GENERATED_BODY()

    // Colors
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Colors")
    FLinearColor PrimaryColor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Colors")
    FLinearColor SecondaryColor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Colors")
    FLinearColor AccentColor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Colors")
    FLinearColor BackgroundColor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Colors")
    FLinearColor TextColor;

    // Fonts and sizes
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Typography")
    class UFont* PrimaryFont;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Typography")
    class UFont* SecondaryFont;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Typography")
    float HeaderFontSize;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Typography")
    float BodyFontSize;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Typography")
    float SmallFontSize;

    // Effects
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
    bool bEnableParticleEffects;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
    bool bEnableGlowEffects;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
    bool bEnableAnimations;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
    float AnimationSpeed;

    // Textures
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Textures")
    class UTexture2D* ButtonTexture;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Textures")
    class UTexture2D* PanelTexture;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Textures")
    class UTexture2D* BorderTexture;

    FAnimeUIStyle()
    {
        PrimaryColor = FLinearColor(0.2f, 0.6f, 1.0f, 1.0f); // Anime blue
        SecondaryColor = FLinearColor(1.0f, 0.8f, 0.2f, 1.0f); // Anime gold
        AccentColor = FLinearColor(1.0f, 0.4f, 0.6f, 1.0f); // Anime pink
        BackgroundColor = FLinearColor(0.05f, 0.05f, 0.1f, 0.9f); // Dark transparent
        TextColor = FLinearColor::White;
        
        PrimaryFont = nullptr;
        SecondaryFont = nullptr;
        HeaderFontSize = 24.0f;
        BodyFontSize = 16.0f;
        SmallFontSize = 12.0f;
        
        bEnableParticleEffects = true;
        bEnableGlowEffects = true;
        bEnableAnimations = true;
        AnimationSpeed = 1.0f;
        
        ButtonTexture = nullptr;
        PanelTexture = nullptr;
        BorderTexture = nullptr;
    }
};

USTRUCT(BlueprintType)
struct FAnimeUINotification
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Notification")
    FText Message;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Notification")
    FLinearColor Color;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Notification")
    float Duration;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Notification")
    class UTexture2D* Icon;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Notification")
    bool bPlaySound;

    FAnimeUINotification()
    {
        Message = FText::FromString("Notification");
        Color = FLinearColor::White;
        Duration = 3.0f;
        Icon = nullptr;
        bPlaySound = true;
    }
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ANIMEWORLDRUNNER_API UAnimeUIManager : public UActorComponent
{
    GENERATED_BODY()

public:
    UAnimeUIManager();

protected:
    virtual void BeginPlay() override;

public:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // UI Management
    UFUNCTION(BlueprintCallable, Category = "Anime UI")
    void ShowUI(EAnimeUIType UIType, bool bAnimate = true);

    UFUNCTION(BlueprintCallable, Category = "Anime UI")
    void HideUI(EAnimeUIType UIType, bool bAnimate = true);

    UFUNCTION(BlueprintCallable, Category = "Anime UI")
    void HideAllUI(bool bAnimate = true);

    UFUNCTION(BlueprintCallable, Category = "Anime UI")
    bool IsUIVisible(EAnimeUIType UIType);

    UFUNCTION(BlueprintCallable, Category = "Anime UI")
    UUserWidget* GetUIWidget(EAnimeUIType UIType);

    // Animation System
    UFUNCTION(BlueprintCallable, Category = "Anime UI")
    void PlayUIAnimation(UUserWidget* Widget, EAnimeUIAnimation AnimationType, float Duration = 0.5f);

    UFUNCTION(BlueprintCallable, Category = "Anime UI")
    void StopUIAnimation(UUserWidget* Widget);

    // Notification System
    UFUNCTION(BlueprintCallable, Category = "Anime UI")
    void ShowNotification(const FAnimeUINotification& Notification);

    UFUNCTION(BlueprintCallable, Category = "Anime UI")
    void ShowSimpleNotification(const FText& Message, float Duration = 3.0f);

    UFUNCTION(BlueprintCallable, Category = "Anime UI")
    void ClearNotifications();

    // Style Management
    UFUNCTION(BlueprintCallable, Category = "Anime UI")
    void ApplyUIStyle(const FAnimeUIStyle& Style);

    UFUNCTION(BlueprintCallable, Category = "Anime UI")
    void SetUITheme(FLinearColor PrimaryColor, FLinearColor SecondaryColor, FLinearColor AccentColor);

    // Mobile Optimization
    UFUNCTION(BlueprintCallable, Category = "Anime UI")
    void OptimizeForMobile(bool bEnable);

    UFUNCTION(BlueprintCallable, Category = "Anime UI")
    void SetUIScale(float Scale);

    // Touch Controls
    UFUNCTION(BlueprintCallable, Category = "Anime UI")
    void ShowVirtualControls(bool bShow);

    UFUNCTION(BlueprintCallable, Category = "Anime UI")
    void UpdateVirtualJoystick(FVector2D InputVector);

    // HUD Updates
    UFUNCTION(BlueprintCallable, Category = "Anime UI")
    void UpdateScore(int32 Score);

    UFUNCTION(BlueprintCallable, Category = "Anime UI")
    void UpdateCoins(int32 Coins);

    UFUNCTION(BlueprintCallable, Category = "Anime UI")
    void UpdateDistance(float Distance);

    UFUNCTION(BlueprintCallable, Category = "Anime UI")
    void UpdateHealth(float Health, float MaxHealth);

    UFUNCTION(BlueprintCallable, Category = "Anime UI")
    void UpdatePowerUp(const FText& PowerUpName, float Duration);

protected:
    // UI Widget Classes
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Classes")
    TMap<EAnimeUIType, TSubclassOf<UUserWidget>> UIWidgetClasses;

    // Active UI Widgets
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Active UI")
    TMap<EAnimeUIType, UUserWidget*> ActiveUIWidgets;

    // UI Style
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI Style")
    FAnimeUIStyle CurrentUIStyle;

    // Notification Queue
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Notifications")
    TArray<FAnimeUINotification> NotificationQueue;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Notifications")
    TArray<UUserWidget*> ActiveNotifications;

    // Animation System
    UPROPERTY()
    TMap<UUserWidget*, FTimerHandle> AnimationTimers;

    // Mobile Settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mobile")
    bool bMobileOptimization;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mobile")
    float MobileUIScale;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mobile")
    bool bShowVirtualControls;

    // Virtual Controls
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Virtual Controls")
    UUserWidget* VirtualJoystickWidget;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Virtual Controls")
    UUserWidget* VirtualButtonsWidget;

    // Sound Effects
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    class USoundCue* UIClickSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    class USoundCue* UIHoverSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    class USoundCue* NotificationSound;

private:
    // Helper functions
    void InitializeUIWidgets();
    void CreateUIWidget(EAnimeUIType UIType);
    void ProcessNotificationQueue();
    void UpdateUIAnimations(float DeltaTime);
    void ApplyMobileOptimizations();
    void SetupVirtualControls();

    // Animation functions
    void AnimateFadeIn(UUserWidget* Widget, float Duration);
    void AnimateFadeOut(UUserWidget* Widget, float Duration);
    void AnimateSlideIn(UUserWidget* Widget, float Duration);
    void AnimateSlideOut(UUserWidget* Widget, float Duration);
    void AnimateScaleIn(UUserWidget* Widget, float Duration);
    void AnimateScaleOut(UUserWidget* Widget, float Duration);
    void AnimateBounce(UUserWidget* Widget, float Duration);
    void AnimateShake(UUserWidget* Widget, float Duration);

    // Notification management
    float NotificationTimer;
    float NotificationSpacing;
};

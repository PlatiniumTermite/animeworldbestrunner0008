#include "UI/AnimeUIManager.h"
#include "Blueprint/UserWidget.h"
#include "Components/Widget.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"
#include "Animation/UMGSequencePlayer.h"
#include "Components/CanvasPanel.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

UAnimeUIManager::UAnimeUIManager()
{
    PrimaryComponentTick.bCanEverTick = true;
    
    // Initialize settings
    bMobileOptimization = true;
    MobileUIScale = 1.2f; // Larger for touch screens
    bShowVirtualControls = true;
    
    NotificationTimer = 0.0f;
    NotificationSpacing = 80.0f;
    
    // Initialize default UI style
    CurrentUIStyle = FAnimeUIStyle();
}

void UAnimeUIManager::BeginPlay()
{
    Super::BeginPlay();
    
    InitializeUIWidgets();
    
    if (bMobileOptimization)
    {
        ApplyMobileOptimizations();
    }
    
    if (bShowVirtualControls)
    {
        SetupVirtualControls();
    }
}

void UAnimeUIManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    
    NotificationTimer += DeltaTime;
    ProcessNotificationQueue();
    UpdateUIAnimations(DeltaTime);
}

void UAnimeUIManager::InitializeUIWidgets()
{
    // Create UI widgets for each type
    for (auto& WidgetClassPair : UIWidgetClasses)
    {
        EAnimeUIType UIType = WidgetClassPair.Key;
        TSubclassOf<UUserWidget> WidgetClass = WidgetClassPair.Value;
        
        if (WidgetClass)
        {
            CreateUIWidget(UIType);
        }
    }
}

void UAnimeUIManager::CreateUIWidget(EAnimeUIType UIType)
{
    if (TSubclassOf<UUserWidget>* WidgetClass = UIWidgetClasses.Find(UIType))
    {
        if (*WidgetClass)
        {
            UUserWidget* NewWidget = CreateWidget<UUserWidget>(GetWorld(), *WidgetClass);
            if (NewWidget)
            {
                ActiveUIWidgets.Add(UIType, NewWidget);
                
                // Initially hide all widgets except HUD
                if (UIType != EAnimeUIType::GameHUD)
                {
                    NewWidget->SetVisibility(ESlateVisibility::Hidden);
                }
                else
                {
                    NewWidget->AddToViewport(100); // High Z-order for HUD
                }
            }
        }
    }
}

void UAnimeUIManager::ShowUI(EAnimeUIType UIType, bool bAnimate)
{
    if (UUserWidget** Widget = ActiveUIWidgets.Find(UIType))
    {
        if (*Widget)
        {
            (*Widget)->AddToViewport();
            (*Widget)->SetVisibility(ESlateVisibility::Visible);
            
            if (bAnimate && CurrentUIStyle.bEnableAnimations)
            {
                PlayUIAnimation(*Widget, EAnimeUIAnimation::FadeIn, 0.3f);
            }
            
            // Play UI sound
            if (UIClickSound)
            {
                UGameplayStatics::PlaySound2D(GetWorld(), UIClickSound);
            }
        }
    }
}

void UAnimeUIManager::HideUI(EAnimeUIType UIType, bool bAnimate)
{
    if (UUserWidget** Widget = ActiveUIWidgets.Find(UIType))
    {
        if (*Widget)
        {
            if (bAnimate && CurrentUIStyle.bEnableAnimations)
            {
                PlayUIAnimation(*Widget, EAnimeUIAnimation::FadeOut, 0.3f);
                
                // Hide after animation completes
                FTimerHandle HideTimer;
                GetWorld()->GetTimerManager().SetTimer(HideTimer, [this, Widget]()
                {
                    if (*Widget)
                    {
                        (*Widget)->RemoveFromParent();
                        (*Widget)->SetVisibility(ESlateVisibility::Hidden);
                    }
                }, 0.3f, false);
            }
            else
            {
                (*Widget)->RemoveFromParent();
                (*Widget)->SetVisibility(ESlateVisibility::Hidden);
            }
        }
    }
}

void UAnimeUIManager::HideAllUI(bool bAnimate)
{
    for (auto& WidgetPair : ActiveUIWidgets)
    {
        EAnimeUIType UIType = WidgetPair.Key;
        
        // Don't hide HUD and notifications
        if (UIType != EAnimeUIType::GameHUD && UIType != EAnimeUIType::Notification)
        {
            HideUI(UIType, bAnimate);
        }
    }
}

bool UAnimeUIManager::IsUIVisible(EAnimeUIType UIType)
{
    if (UUserWidget** Widget = ActiveUIWidgets.Find(UIType))
    {
        return *Widget && (*Widget)->GetVisibility() == ESlateVisibility::Visible;
    }
    return false;
}

UUserWidget* UAnimeUIManager::GetUIWidget(EAnimeUIType UIType)
{
    if (UUserWidget** Widget = ActiveUIWidgets.Find(UIType))
    {
        return *Widget;
    }
    return nullptr;
}

void UAnimeUIManager::PlayUIAnimation(UUserWidget* Widget, EAnimeUIAnimation AnimationType, float Duration)
{
    if (!Widget || !CurrentUIStyle.bEnableAnimations) return;
    
    // Stop any existing animation
    StopUIAnimation(Widget);
    
    Duration *= CurrentUIStyle.AnimationSpeed;
    
    switch (AnimationType)
    {
        case EAnimeUIAnimation::FadeIn:
            AnimateFadeIn(Widget, Duration);
            break;
        case EAnimeUIAnimation::FadeOut:
            AnimateFadeOut(Widget, Duration);
            break;
        case EAnimeUIAnimation::SlideIn:
            AnimateSlideIn(Widget, Duration);
            break;
        case EAnimeUIAnimation::SlideOut:
            AnimateSlideOut(Widget, Duration);
            break;
        case EAnimeUIAnimation::ScaleIn:
            AnimateScaleIn(Widget, Duration);
            break;
        case EAnimeUIAnimation::ScaleOut:
            AnimateScaleOut(Widget, Duration);
            break;
        case EAnimeUIAnimation::Bounce:
            AnimateBounce(Widget, Duration);
            break;
        case EAnimeUIAnimation::Shake:
            AnimateShake(Widget, Duration);
            break;
    }
}

void UAnimeUIManager::StopUIAnimation(UUserWidget* Widget)
{
    if (FTimerHandle* Timer = AnimationTimers.Find(Widget))
    {
        GetWorld()->GetTimerManager().ClearTimer(*Timer);
        AnimationTimers.Remove(Widget);
    }
}

void UAnimeUIManager::ShowNotification(const FAnimeUINotification& Notification)
{
    NotificationQueue.Add(Notification);
}

void UAnimeUIManager::ShowSimpleNotification(const FText& Message, float Duration)
{
    FAnimeUINotification SimpleNotification;
    SimpleNotification.Message = Message;
    SimpleNotification.Duration = Duration;
    SimpleNotification.Color = CurrentUIStyle.AccentColor;
    
    ShowNotification(SimpleNotification);
}

void UAnimeUIManager::ProcessNotificationQueue()
{
    if (NotificationQueue.Num() > 0 && NotificationTimer >= 0.5f) // Stagger notifications
    {
        FAnimeUINotification NextNotification = NotificationQueue[0];
        NotificationQueue.RemoveAt(0);
        
        // Create notification widget (this would be implemented in Blueprint)
        if (TSubclassOf<UUserWidget>* NotificationClass = UIWidgetClasses.Find(EAnimeUIType::Notification))
        {
            if (*NotificationClass)
            {
                UUserWidget* NotificationWidget = CreateWidget<UUserWidget>(GetWorld(), *NotificationClass);
                if (NotificationWidget)
                {
                    // Position notification
                    float YOffset = ActiveNotifications.Num() * NotificationSpacing;
                    NotificationWidget->AddToViewport(200); // High Z-order
                    
                    // Set notification data (would be done through binding in Blueprint)
                    // This is a simplified version
                    
                    ActiveNotifications.Add(NotificationWidget);
                    
                    // Animate in
                    PlayUIAnimation(NotificationWidget, EAnimeUIAnimation::SlideIn, 0.5f);
                    
                    // Play notification sound
                    if (NextNotification.bPlaySound && NotificationSound)
                    {
                        UGameplayStatics::PlaySound2D(GetWorld(), NotificationSound);
                    }
                    
                    // Auto-remove after duration
                    FTimerHandle RemoveTimer;
                    GetWorld()->GetTimerManager().SetTimer(RemoveTimer, [this, NotificationWidget]()
                    {
                        if (NotificationWidget)
                        {
                            PlayUIAnimation(NotificationWidget, EAnimeUIAnimation::SlideOut, 0.3f);
                            
                            FTimerHandle DestroyTimer;
                            GetWorld()->GetTimerManager().SetTimer(DestroyTimer, [this, NotificationWidget]()
                            {
                                ActiveNotifications.Remove(NotificationWidget);
                                NotificationWidget->RemoveFromParent();
                            }, 0.3f, false);
                        }
                    }, NextNotification.Duration, false);
                }
            }
        }
        
        NotificationTimer = 0.0f;
    }
}

void UAnimeUIManager::ClearNotifications()
{
    NotificationQueue.Empty();
    
    for (UUserWidget* Notification : ActiveNotifications)
    {
        if (Notification)
        {
            Notification->RemoveFromParent();
        }
    }
    ActiveNotifications.Empty();
}

void UAnimeUIManager::ApplyUIStyle(const FAnimeUIStyle& Style)
{
    CurrentUIStyle = Style;
    
    // Apply style to all active widgets
    for (auto& WidgetPair : ActiveUIWidgets)
    {
        UUserWidget* Widget = WidgetPair.Value;
        if (Widget)
        {
            // This would typically be done through data binding in Blueprint
            // Here we're showing the concept
        }
    }
}

void UAnimeUIManager::SetUITheme(FLinearColor PrimaryColor, FLinearColor SecondaryColor, FLinearColor AccentColor)
{
    CurrentUIStyle.PrimaryColor = PrimaryColor;
    CurrentUIStyle.SecondaryColor = SecondaryColor;
    CurrentUIStyle.AccentColor = AccentColor;
    
    ApplyUIStyle(CurrentUIStyle);
}

void UAnimeUIManager::OptimizeForMobile(bool bEnable)
{
    bMobileOptimization = bEnable;
    
    if (bEnable)
    {
        ApplyMobileOptimizations();
    }
}

void UAnimeUIManager::ApplyMobileOptimizations()
{
    // Reduce animation complexity
    CurrentUIStyle.bEnableParticleEffects = false;
    CurrentUIStyle.AnimationSpeed = 1.5f; // Faster animations for mobile
    
    // Increase UI scale for touch
    SetUIScale(MobileUIScale);
    
    // Simplify effects
    CurrentUIStyle.bEnableGlowEffects = false;
}

void UAnimeUIManager::SetUIScale(float Scale)
{
    MobileUIScale = Scale;
    
    // Apply scale to all widgets
    for (auto& WidgetPair : ActiveUIWidgets)
    {
        UUserWidget* Widget = WidgetPair.Value;
        if (Widget)
        {
            Widget->SetRenderScale(FVector2D(Scale, Scale));
        }
    }
}

void UAnimeUIManager::ShowVirtualControls(bool bShow)
{
    bShowVirtualControls = bShow;
    
    if (VirtualJoystickWidget)
    {
        VirtualJoystickWidget->SetVisibility(bShow ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
    }
    
    if (VirtualButtonsWidget)
    {
        VirtualButtonsWidget->SetVisibility(bShow ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
    }
}

void UAnimeUIManager::SetupVirtualControls()
{
    // This would typically create virtual joystick and button widgets
    // Implementation would be done in Blueprint with proper touch handling
}

void UAnimeUIManager::UpdateVirtualJoystick(FVector2D InputVector)
{
    // Update virtual joystick visual position
    // This would be implemented in Blueprint
}

// HUD Update functions
void UAnimeUIManager::UpdateScore(int32 Score)
{
    if (UUserWidget* HUD = GetUIWidget(EAnimeUIType::GameHUD))
    {
        // Update score display - would be done through data binding in Blueprint
    }
}

void UAnimeUIManager::UpdateCoins(int32 Coins)
{
    if (UUserWidget* HUD = GetUIWidget(EAnimeUIType::GameHUD))
    {
        // Update coin display - would be done through data binding in Blueprint
        
        // Show coin collection effect
        if (CurrentUIStyle.bEnableAnimations)
        {
            PlayUIAnimation(HUD, EAnimeUIAnimation::Bounce, 0.2f);
        }
    }
}

void UAnimeUIManager::UpdateDistance(float Distance)
{
    if (UUserWidget* HUD = GetUIWidget(EAnimeUIType::GameHUD))
    {
        // Update distance display - would be done through data binding in Blueprint
    }
}

void UAnimeUIManager::UpdateHealth(float Health, float MaxHealth)
{
    if (UUserWidget* HUD = GetUIWidget(EAnimeUIType::GameHUD))
    {
        // Update health bar - would be done through data binding in Blueprint
        
        // Show damage effect if health decreased
        static float LastHealth = MaxHealth;
        if (Health < LastHealth)
        {
            PlayUIAnimation(HUD, EAnimeUIAnimation::Shake, 0.3f);
        }
        LastHealth = Health;
    }
}

void UAnimeUIManager::UpdatePowerUp(const FText& PowerUpName, float Duration)
{
    FAnimeUINotification PowerUpNotification;
    PowerUpNotification.Message = FText::Format(FText::FromString("Power-Up: {0}"), PowerUpName);
    PowerUpNotification.Duration = Duration;
    PowerUpNotification.Color = CurrentUIStyle.SecondaryColor;
    PowerUpNotification.bPlaySound = true;
    
    ShowNotification(PowerUpNotification);
}

// Animation implementations
void UAnimeUIManager::AnimateFadeIn(UUserWidget* Widget, float Duration)
{
    Widget->SetRenderOpacity(0.0f);
    
    FTimerHandle FadeTimer;
    float ElapsedTime = 0.0f;
    
    GetWorld()->GetTimerManager().SetTimer(FadeTimer, [this, Widget, Duration, ElapsedTime]() mutable
    {
        ElapsedTime += 0.016f; // ~60 FPS
        float Alpha = FMath::Clamp(ElapsedTime / Duration, 0.0f, 1.0f);
        
        if (Widget)
        {
            Widget->SetRenderOpacity(Alpha);
        }
        
        if (Alpha >= 1.0f)
        {
            GetWorld()->GetTimerManager().ClearTimer(AnimationTimers[Widget]);
            AnimationTimers.Remove(Widget);
        }
    }, 0.016f, true);
    
    AnimationTimers.Add(Widget, FadeTimer);
}

void UAnimeUIManager::AnimateFadeOut(UUserWidget* Widget, float Duration)
{
    Widget->SetRenderOpacity(1.0f);
    
    FTimerHandle FadeTimer;
    float ElapsedTime = 0.0f;
    
    GetWorld()->GetTimerManager().SetTimer(FadeTimer, [this, Widget, Duration, ElapsedTime]() mutable
    {
        ElapsedTime += 0.016f;
        float Alpha = FMath::Clamp(1.0f - (ElapsedTime / Duration), 0.0f, 1.0f);
        
        if (Widget)
        {
            Widget->SetRenderOpacity(Alpha);
        }
        
        if (Alpha <= 0.0f)
        {
            GetWorld()->GetTimerManager().ClearTimer(AnimationTimers[Widget]);
            AnimationTimers.Remove(Widget);
        }
    }, 0.016f, true);
    
    AnimationTimers.Add(Widget, FadeTimer);
}

void UAnimeUIManager::AnimateScaleIn(UUserWidget* Widget, float Duration)
{
    Widget->SetRenderScale(FVector2D::ZeroVector);
    
    FTimerHandle ScaleTimer;
    float ElapsedTime = 0.0f;
    
    GetWorld()->GetTimerManager().SetTimer(ScaleTimer, [this, Widget, Duration, ElapsedTime]() mutable
    {
        ElapsedTime += 0.016f;
        float Scale = FMath::Clamp(ElapsedTime / Duration, 0.0f, 1.0f);
        
        // Ease out for smooth animation
        Scale = 1.0f - FMath::Pow(1.0f - Scale, 3.0f);
        
        if (Widget)
        {
            Widget->SetRenderScale(FVector2D(Scale * MobileUIScale, Scale * MobileUIScale));
        }
        
        if (Scale >= 1.0f)
        {
            GetWorld()->GetTimerManager().ClearTimer(AnimationTimers[Widget]);
            AnimationTimers.Remove(Widget);
        }
    }, 0.016f, true);
    
    AnimationTimers.Add(Widget, ScaleTimer);
}

void UAnimeUIManager::AnimateScaleOut(UUserWidget* Widget, float Duration)
{
    Widget->SetRenderScale(FVector2D(MobileUIScale, MobileUIScale));
    
    FTimerHandle ScaleTimer;
    float ElapsedTime = 0.0f;
    
    GetWorld()->GetTimerManager().SetTimer(ScaleTimer, [this, Widget, Duration, ElapsedTime]() mutable
    {
        ElapsedTime += 0.016f;
        float Scale = FMath::Clamp(1.0f - (ElapsedTime / Duration), 0.0f, 1.0f);
        
        if (Widget)
        {
            Widget->SetRenderScale(FVector2D(Scale * MobileUIScale, Scale * MobileUIScale));
        }
        
        if (Scale <= 0.0f)
        {
            GetWorld()->GetTimerManager().ClearTimer(AnimationTimers[Widget]);
            AnimationTimers.Remove(Widget);
        }
    }, 0.016f, true);
    
    AnimationTimers.Add(Widget, ScaleTimer);
}

void UAnimeUIManager::AnimateBounce(UUserWidget* Widget, float Duration)
{
    FTimerHandle BounceTimer;
    float ElapsedTime = 0.0f;
    FVector2D OriginalScale = Widget->GetRenderTransform().Scale;
    
    GetWorld()->GetTimerManager().SetTimer(BounceTimer, [this, Widget, Duration, ElapsedTime, OriginalScale]() mutable
    {
        ElapsedTime += 0.016f;
        float Progress = ElapsedTime / Duration;
        
        // Bounce effect using sine wave
        float BounceScale = 1.0f + FMath::Sin(Progress * PI * 4.0f) * 0.1f * (1.0f - Progress);
        
        if (Widget)
        {
            Widget->SetRenderScale(OriginalScale * BounceScale);
        }
        
        if (Progress >= 1.0f)
        {
            Widget->SetRenderScale(OriginalScale);
            GetWorld()->GetTimerManager().ClearTimer(AnimationTimers[Widget]);
            AnimationTimers.Remove(Widget);
        }
    }, 0.016f, true);
    
    AnimationTimers.Add(Widget, BounceTimer);
}

void UAnimeUIManager::AnimateShake(UUserWidget* Widget, float Duration)
{
    FTimerHandle ShakeTimer;
    float ElapsedTime = 0.0f;
    FVector2D OriginalPosition = Widget->GetRenderTransform().Translation;
    
    GetWorld()->GetTimerManager().SetTimer(ShakeTimer, [this, Widget, Duration, ElapsedTime, OriginalPosition]() mutable
    {
        ElapsedTime += 0.016f;
        float Progress = ElapsedTime / Duration;
        
        // Shake effect with decreasing intensity
        float ShakeIntensity = 10.0f * (1.0f - Progress);
        FVector2D ShakeOffset = FVector2D(
            FMath::RandRange(-ShakeIntensity, ShakeIntensity),
            FMath::RandRange(-ShakeIntensity, ShakeIntensity)
        );
        
        if (Widget)
        {
            Widget->SetRenderTranslation(OriginalPosition + ShakeOffset);
        }
        
        if (Progress >= 1.0f)
        {
            Widget->SetRenderTranslation(OriginalPosition);
            GetWorld()->GetTimerManager().ClearTimer(AnimationTimers[Widget]);
            AnimationTimers.Remove(Widget);
        }
    }, 0.016f, true);
    
    AnimationTimers.Add(Widget, ShakeTimer);
}

void UAnimeUIManager::AnimateSlideIn(UUserWidget* Widget, float Duration)
{
    // Slide in from right
    FVector2D StartPosition = FVector2D(1920.0f, 0.0f); // Off-screen right
    FVector2D EndPosition = FVector2D::ZeroVector;
    
    Widget->SetRenderTranslation(StartPosition);
    
    FTimerHandle SlideTimer;
    float ElapsedTime = 0.0f;
    
    GetWorld()->GetTimerManager().SetTimer(SlideTimer, [this, Widget, Duration, ElapsedTime, StartPosition, EndPosition]() mutable
    {
        ElapsedTime += 0.016f;
        float Alpha = FMath::Clamp(ElapsedTime / Duration, 0.0f, 1.0f);
        
        // Ease out for smooth animation
        Alpha = 1.0f - FMath::Pow(1.0f - Alpha, 3.0f);
        
        FVector2D CurrentPosition = FMath::Lerp(StartPosition, EndPosition, Alpha);
        
        if (Widget)
        {
            Widget->SetRenderTranslation(CurrentPosition);
        }
        
        if (Alpha >= 1.0f)
        {
            GetWorld()->GetTimerManager().ClearTimer(AnimationTimers[Widget]);
            AnimationTimers.Remove(Widget);
        }
    }, 0.016f, true);
    
    AnimationTimers.Add(Widget, SlideTimer);
}

void UAnimeUIManager::AnimateSlideOut(UUserWidget* Widget, float Duration)
{
    // Slide out to left
    FVector2D StartPosition = Widget->GetRenderTransform().Translation;
    FVector2D EndPosition = FVector2D(-1920.0f, 0.0f); // Off-screen left
    
    FTimerHandle SlideTimer;
    float ElapsedTime = 0.0f;
    
    GetWorld()->GetTimerManager().SetTimer(SlideTimer, [this, Widget, Duration, ElapsedTime, StartPosition, EndPosition]() mutable
    {
        ElapsedTime += 0.016f;
        float Alpha = FMath::Clamp(ElapsedTime / Duration, 0.0f, 1.0f);
        
        FVector2D CurrentPosition = FMath::Lerp(StartPosition, EndPosition, Alpha);
        
        if (Widget)
        {
            Widget->SetRenderTranslation(CurrentPosition);
        }
        
        if (Alpha >= 1.0f)
        {
            GetWorld()->GetTimerManager().ClearTimer(AnimationTimers[Widget]);
            AnimationTimers.Remove(Widget);
        }
    }, 0.016f, true);
    
    AnimationTimers.Add(Widget, SlideTimer);
}

void UAnimeUIManager::UpdateUIAnimations(float DeltaTime)
{
    // This function handles any continuous UI animations
    // Most animations are handled by timers, but this can be used for
    // continuous effects like glow pulsing, etc.
}

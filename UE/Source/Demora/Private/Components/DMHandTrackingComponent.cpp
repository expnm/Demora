// exp.dev free

#include "Components/DMHandTrackingComponent.h"
#include "DMReceiver.h"

DEFINE_LOG_CATEGORY_STATIC(LogDMHandTrackingComponent, All, All);

UDMHandTrackingComponent::UDMHandTrackingComponent()
{
#if WITH_EDITOR
    if (GEditor)
    {
        GEditor->GetTimerManager()->SetTimerForNextTick(this, &UDMHandTrackingComponent::Initialize);
    }
#endif // WITH_EDITOR
}

void UDMHandTrackingComponent::BeginPlay()
{
    Super::BeginPlay();

#if !WITH_EDITOR
    Initialize();
#endif // !WITH_EDITOR
}

void UDMHandTrackingComponent::Initialize()
{
    if (!bActive) return;

#if WITH_EDITOR
    GEditor->GetTimerManager()->SetTimer(
        UpdateCurrentValuesTimerHandle, this, &UDMHandTrackingComponent::UpdateCurrentValues, TickInSeconds, true);
#else
    GetWorld()->GetTimerManager().SetTimer(
        UpdateCurrentValuesTimerHandle, this, &UDMHandTrackingComponent::UpdateCurrentValues, TickInSeconds, true);
#endif // WITH_EDITOR

    if (MediapipeReceiver && MediapipeReceiver->IsActive())
    {
        MediapipeReceiver->Updated.AddDynamic(this, &UDMHandTrackingComponent::UpdateTargetValues);
    }
}

void UDMHandTrackingComponent::UpdateTargetValues(const FString& InMessage)
{
    TArray<FString> Values;
    InMessage.ParseIntoArray(Values, TEXT(","), true);
    const int LandmarksCount = (int)EDMHandLandmark::MAX;
    if (Values.Num() != LandmarksCount * 2) return;

    if (TargetLeft.IsEmpty()) TargetLeft.Init(0, LandmarksCount);
    if (TargetRight.IsEmpty()) TargetRight.Init(0, LandmarksCount);

    for (int i = 0; i < LandmarksCount; ++i)
    {
        TargetLeft[i] = FCString::Atof(*Values[TrackingSettings.bFlipTracking ? i + LandmarksCount : i]);
        TargetRight[i] = FCString::Atof(*Values[TrackingSettings.bFlipTracking ? i : i + LandmarksCount]);
    }
}

void UDMHandTrackingComponent::UpdateCurrentValues()
{
    if (CurrentLeft.IsEmpty()) CurrentLeft = TargetLeft;
    if (CurrentRight.IsEmpty()) CurrentRight = TargetRight;
    if (CurrentLeft.IsEmpty() || CurrentRight.IsEmpty()) return;

    bool bHasChangesLeft = false;
    bool bHasChangesRight = false;

    for (int i = 0; i < (int)EDMHandLandmark::MAX; ++i)
    {
        if (!FMath::IsNearlyEqual(CurrentLeft[i], TargetLeft[i], ErrorTolerance))
        {
            TrackingData.bLostLeft = false;
            bHasChangesLeft = true;
            CurrentLeft[i] += (TargetLeft[i] - CurrentLeft[i]) * TickMultiply;
        }

        if (!FMath::IsNearlyEqual(CurrentRight[i], TargetRight[i], ErrorTolerance))
        {
            TrackingData.bLostRight = false;
            bHasChangesRight = true;
            CurrentRight[i] += (TargetRight[i] - CurrentRight[i]) * TickMultiply;
        }
    }

#if WITH_EDITOR
    UpdateFocusLostTimerState(bHasChangesLeft, bHasChangesRight, *GEditor->GetTimerManager());
#else
    UpdateFocusLostTimerState(bHasChangesLeft, bHasChangesRight, GetWorld()->GetTimerManager());
#endif // WITH_EDITOR

    if (bHasChangesLeft)
    {
        TrackingData.Calculate(false, TrackingSettings,
            FRotator(CurrentLeft[(int)EDMHandLandmark::HandPitch], CurrentLeft[(int)EDMHandLandmark::HandYaw],
                CurrentLeft[(int)EDMHandLandmark::HandRoll]),
            FVector(CurrentLeft[(int)EDMHandLandmark::HandX], CurrentLeft[(int)EDMHandLandmark::HandY],
                CurrentLeft[(int)EDMHandLandmark::HandZ]),
            CurrentLeft[(int)EDMHandLandmark::IndexFinger], CurrentLeft[(int)EDMHandLandmark::MiddleFinger],
            CurrentLeft[(int)EDMHandLandmark::RingFinger], CurrentLeft[(int)EDMHandLandmark::PinkyFinger],
            CurrentLeft[(int)EDMHandLandmark::ThumbFinger]);
    }

    if (bHasChangesRight)
    {
        TrackingData.Calculate(true, TrackingSettings,
            FRotator(CurrentRight[(int)EDMHandLandmark::HandPitch], CurrentRight[(int)EDMHandLandmark::HandYaw],
                CurrentRight[(int)EDMHandLandmark::HandRoll]),
            FVector(CurrentRight[(int)EDMHandLandmark::HandX], CurrentRight[(int)EDMHandLandmark::HandY],
                CurrentRight[(int)EDMHandLandmark::HandZ]),
            CurrentRight[(int)EDMHandLandmark::IndexFinger], CurrentRight[(int)EDMHandLandmark::MiddleFinger],
            CurrentRight[(int)EDMHandLandmark::RingFinger], CurrentRight[(int)EDMHandLandmark::PinkyFinger],
            CurrentRight[(int)EDMHandLandmark::ThumbFinger]);
    }
}

void UDMHandTrackingComponent::UpdateFocusLostTimerState(
    const bool bHasChangesLeft, const bool bHasChangesRight, FTimerManager& InTimerManager)
{
    if (FMath::IsNearlyZero(FocusLostTimeInSeconds)) return;

    const bool bTimerActiveFocusLostLeft = InTimerManager.IsTimerActive(FocusLostLeftTimerHandle);
    const bool bTimerActiveFocusLostRight = InTimerManager.IsTimerActive(FocusLostRightTimerHandle);

    // если пришли свежие данные И таймер активен
    if (bHasChangesLeft && bTimerActiveFocusLostLeft) InTimerManager.ClearTimer(FocusLostLeftTimerHandle);
    if (bHasChangesRight && bTimerActiveFocusLostRight) InTimerManager.ClearTimer(FocusLostRightTimerHandle);

    // если нет изменений И таймер не запущен И фокус активен
    if (!bHasChangesLeft && !bTimerActiveFocusLostLeft && !TrackingData.bLostLeft)
    {
        InTimerManager.SetTimer(
            FocusLostLeftTimerHandle, this, &UDMHandTrackingComponent::FocusLostByTimerLeft, FocusLostTimeInSeconds);
    }

    if (!bHasChangesRight && !bTimerActiveFocusLostRight && !TrackingData.bLostRight)
    {
        InTimerManager.SetTimer(
            FocusLostRightTimerHandle, this, &UDMHandTrackingComponent::FocusLostByTimerRight, FocusLostTimeInSeconds);
    }
}

void UDMHandTrackingComponent::FocusLostByTimerLeft()
{
    TrackingData.bLostLeft = true;
    OnFocusLostLeft.Broadcast();
    UE_LOG(LogDMHandTrackingComponent, Warning, TEXT("Focus lost for left"));
}

void UDMHandTrackingComponent::FocusLostByTimerRight()
{
    TrackingData.bLostRight = true;
    OnFocusLostRight.Broadcast();
    UE_LOG(LogDMHandTrackingComponent, Warning, TEXT("Focus lost for right"));
}

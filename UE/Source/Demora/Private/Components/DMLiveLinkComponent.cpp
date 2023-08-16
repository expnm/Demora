// exp.dev free

#include "Components/DMLiveLinkComponent.h"
#include "ARTrackable.h"
#include "DMReceiver.h"
#include "Roles/LiveLinkBasicRole.h"

DEFINE_LOG_CATEGORY_STATIC(LogDMLiveLinkComponent, All, All);

UDMLiveLinkComponent::UDMLiveLinkComponent()
{
    IModularFeatures& ModularFeatures = IModularFeatures::Get();

    if (ModularFeatures.IsModularFeatureAvailable(ILiveLinkClient::ModularFeatureName))
    {
        LiveLinkClient = &ModularFeatures.GetModularFeature<ILiveLinkClient>(ILiveLinkClient::ModularFeatureName);
        OnLiveLinkUpdated.AddDynamic(this, &UDMLiveLinkComponent::LiveLinkUpdated);
    }

#if WITH_EDITOR
    if (GEditor)
    {
        GEditor->GetTimerManager()->SetTimerForNextTick(this, &UDMLiveLinkComponent::Initialize);
    }
#endif // WITH_EDITOR
}

void UDMLiveLinkComponent::BeginPlay()
{
    Super::BeginPlay();

#if !WITH_EDITOR
    Initialize();
#endif // !WITH_EDITOR
}

void UDMLiveLinkComponent::Initialize()
{
    if (!bActive) return;

    const AActor* Owner = GetOwner();
    if (!Owner) return;

    BodySkeletalMeshComponent = Owner->FindComponentByClass<USkeletalMeshComponent>();

    // mediapipe
    if (MediapipeReceiver && MediapipeReceiver->IsActive())
    {
        MediapipeReceiver->Updated.AddDynamic(this, &UDMLiveLinkComponent::MediapipeUpdated);
    }
}

void UDMLiveLinkComponent::LiveLinkUpdated(float DeltaTime)
{
    if (!bActive)
    {
        OnLiveLinkUpdated.Clear();
        return;
    }

    if (TrackingType != EDMTrackingType::Default) return;

    TArray<float> RawFrameValues;
    FLiveLinkSubjectFrameData SubjectData;
    if (LiveLinkClient->EvaluateFrame_AnyThread(
            FLiveLinkSubjectName(SubjectName), ULiveLinkBasicRole::StaticClass(), SubjectData))
    {
        FLiveLinkBaseFrameData* FrameData = SubjectData.FrameData.Cast<FLiveLinkBaseFrameData>();
        if (FrameData)
        {
            for (const float PropertyValue : FrameData->PropertyValues)
            {
                RawFrameValues.Emplace(PropertyValue);
            }
        }
    }

    UpdateData(RawFrameValues);
}

void UDMLiveLinkComponent::MediapipeUpdated(const FString& InMessage)
{
    if (TrackingType != EDMTrackingType::Mediapipe) return;

    TArray<FString> Values;
    InMessage.ParseIntoArray(Values, TEXT(","), true);
    if (Values.Num() != (int)EARFaceBlendShape::MAX) return;

    TArray<float> RawFrameValues;
    for (int i = 0; i < (int)EARFaceBlendShape::MAX; ++i)
    {
        RawFrameValues.Emplace(FCString::Atof(*Values[i]));
    }

    AsyncTask(ENamedThreads::GameThread, [this, RawFrameValues]() { UpdateData(RawFrameValues); });
}

void UDMLiveLinkComponent::UpdateData(const TArray<float>& InRawFrameValues)
{
    if (InRawFrameValues.Num() != (int)EARFaceBlendShape::MAX) return;

    bool bHasChanges = false;
    TArray<float> UpdatedFrameValues;

    for (int i = 0; i < InRawFrameValues.Num(); ++i)
    {
        const FDMCalibrationBlendShape* CalibrationBlendShape =
            CalibrationSettings.BlendShapes.FindByPredicate([i](const FDMCalibrationBlendShape& CalibrationBlendShape)
                { return CalibrationBlendShape.DefaultType == (EARFaceBlendShape)i; });

        const float CalibrationMultiply = CalibrationBlendShape ? CalibrationBlendShape->Multiply : 1.f;
        UpdatedFrameValues.Emplace(FMath::Clamp(InRawFrameValues[i] * CalibrationMultiply, -1.f, 1.f));

        if (!PreviousFrameValues.IsEmpty() && PreviousFrameValues[i] == UpdatedFrameValues[i])
        {
            continue;
        }

        SetMorphTarget(CalibrationBlendShape->DefaultType, InRawFrameValues[i], CalibrationBlendShape->CustomName,
            UpdatedFrameValues[i]);

        if (!bHasChanges) bHasChanges = true;
    }

    PreviousFrameValues = UpdatedFrameValues;
    UpdateExtendedData(UpdatedFrameValues);

#if WITH_EDITOR
    UpdateFocusLostTimerState(bHasChanges, *GEditor->GetTimerManager());
#else
    UpdateFocusLostTimerState(bHasChanges, GetWorld()->GetTimerManager());
#endif // WITH_EDITOR
}

void UDMLiveLinkComponent::SetMorphTarget(
    const EARFaceBlendShape& InDefaultType, const float DefaultValue, FName UpdateName, float UpdateValue)
{
    check(BodySkeletalMeshComponent);
    BodySkeletalMeshComponent->SetMorphTarget(UpdateName, UpdateValue);
}

void UDMLiveLinkComponent::UpdateFocusLostTimerState(const bool bHasChanges, FTimerManager& InTimerManager)
{
    if (FMath::IsNearlyZero(FocusLostTimeInSeconds)) return;

    // если фокус потерян И пришли свежие данные
    if (IsFocusLost() && bHasChanges)
    {
        bFocusLost = false;
    }

    if (InTimerManager.IsTimerActive(FocusLostTimerHandle))
    {
        // если фокус был потерян ИЛИ пришли свежие данные
        if (IsFocusLost() || bHasChanges)
        {
            InTimerManager.ClearTimer(FocusLostTimerHandle);
        }
    }

    // если фокус активен И нет свежих данных
    else if (!IsFocusLost() && !bHasChanges)
    {
        InTimerManager.SetTimer(
            FocusLostTimerHandle, this, &UDMLiveLinkComponent::FocusLostByTimer, FocusLostTimeInSeconds);
    }
}

void UDMLiveLinkComponent::FocusLostByTimer()
{
    bFocusLost = true;
    OnFocusLost.Broadcast();
    UE_LOG(LogDMLiveLinkComponent, Warning, TEXT("Focus lost"));
}

void UDMLiveLinkComponent::UpdateExtendedData(const TArray<float>& InFrameValues)
{
    ExtendedData.bTongueOut = InFrameValues[(int)EARFaceBlendShape::TongueOut] > CalibrationSettings.TongueOutValue;

    const TRange<float> DefaultRange = TRange<float>(-1.f, 1.f);
    const int FlipMultiply = CalibrationSettings.bFlipTracking ? -1.f : 1.f;

    // head
    ExtendedData.HeadRotation.Roll = FMath::GetMappedRangeValueClamped(DefaultRange,
        TRange<float>(-CalibrationSettings.HeadAngleRotation.Roll + CalibrationSettings.HeadAngleOffset.Roll,
            CalibrationSettings.HeadAngleRotation.Roll + CalibrationSettings.HeadAngleOffset.Roll),
        InFrameValues[(int)EARFaceBlendShape::HeadRoll] * FlipMultiply);

    ExtendedData.HeadRotation.Pitch = FMath::GetMappedRangeValueClamped(DefaultRange,
        TRange<float>(-CalibrationSettings.HeadAngleRotation.Pitch + CalibrationSettings.HeadAngleOffset.Pitch,
            CalibrationSettings.HeadAngleRotation.Pitch + CalibrationSettings.HeadAngleOffset.Pitch),
        InFrameValues[(int)EARFaceBlendShape::HeadPitch]);

    ExtendedData.HeadRotation.Yaw = FMath::GetMappedRangeValueClamped(DefaultRange,
        TRange<float>(-CalibrationSettings.HeadAngleRotation.Yaw + CalibrationSettings.HeadAngleOffset.Yaw,
            CalibrationSettings.HeadAngleRotation.Yaw + CalibrationSettings.HeadAngleOffset.Yaw),
        InFrameValues[(int)EARFaceBlendShape::HeadYaw] * FlipMultiply);

    // left eye
    ExtendedData.LeftEyeRotation.Roll = FMath::GetMappedRangeValueClamped(DefaultRange,
        TRange<float>(-CalibrationSettings.LeftEyeAngleRotation.Roll + CalibrationSettings.LeftEyeAngleOffset.Roll,
            CalibrationSettings.LeftEyeAngleRotation.Roll + CalibrationSettings.LeftEyeAngleOffset.Roll),
        InFrameValues[(int)EARFaceBlendShape::LeftEyeRoll]);

    ExtendedData.LeftEyeRotation.Pitch = FMath::GetMappedRangeValueClamped(DefaultRange,
        TRange<float>(-CalibrationSettings.LeftEyeAngleRotation.Pitch + CalibrationSettings.LeftEyeAngleOffset.Pitch,
            CalibrationSettings.LeftEyeAngleRotation.Pitch + CalibrationSettings.LeftEyeAngleOffset.Pitch),
        InFrameValues[(int)EARFaceBlendShape::LeftEyePitch]);

    ExtendedData.LeftEyeRotation.Yaw = FMath::GetMappedRangeValueClamped(DefaultRange,
        TRange<float>(-CalibrationSettings.LeftEyeAngleRotation.Yaw + CalibrationSettings.LeftEyeAngleOffset.Yaw,
            CalibrationSettings.LeftEyeAngleRotation.Yaw + CalibrationSettings.LeftEyeAngleOffset.Yaw),
        InFrameValues[(int)EARFaceBlendShape::LeftEyeYaw] * FlipMultiply);

    // Right eye
    ExtendedData.RightEyeRotation.Roll = FMath::GetMappedRangeValueClamped(DefaultRange,
        TRange<float>(-CalibrationSettings.RightEyeAngleRotation.Roll + CalibrationSettings.RightEyeAngleOffset.Roll,
            CalibrationSettings.RightEyeAngleRotation.Roll + CalibrationSettings.RightEyeAngleOffset.Roll),
        InFrameValues[(int)EARFaceBlendShape::RightEyeRoll]);

    ExtendedData.RightEyeRotation.Pitch = FMath::GetMappedRangeValueClamped(DefaultRange,
        TRange<float>(-CalibrationSettings.RightEyeAngleRotation.Pitch + CalibrationSettings.RightEyeAngleOffset.Pitch,
            CalibrationSettings.RightEyeAngleRotation.Pitch + CalibrationSettings.RightEyeAngleOffset.Pitch),
        InFrameValues[(int)EARFaceBlendShape::RightEyePitch]);

    ExtendedData.RightEyeRotation.Yaw = FMath::GetMappedRangeValueClamped(DefaultRange,
        TRange<float>(-CalibrationSettings.RightEyeAngleRotation.Yaw + CalibrationSettings.RightEyeAngleOffset.Yaw,
            CalibrationSettings.RightEyeAngleRotation.Yaw + CalibrationSettings.RightEyeAngleOffset.Yaw),
        InFrameValues[(int)EARFaceBlendShape::RightEyeYaw] * FlipMultiply);
}

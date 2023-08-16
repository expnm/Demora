// exp.dev free

#pragma once

#include "CoreMinimal.h"
#include "LiveLinkComponent.h"
#include "Types/DMLiveLinkTypes.h"
#include "Types/DMStringLiterals.h"

#include "DMLiveLinkComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFocusLost);

class ADMReceiver;

UCLASS(ClassGroup = (DM), meta = (BlueprintSpawnableComponent, DisplayName = "LiveLinkComponent"))
class DEMORA_API UDMLiveLinkComponent : public ULiveLinkComponent
{
    GENERATED_BODY()

public:
    UDMLiveLinkComponent();

    UFUNCTION(BlueprintCallable)
    bool IsFocusLost() const { return bFocusLost; }

    UFUNCTION(BlueprintCallable)
    FDMLiveLinkExtendedData GetExtendedData() const { return ExtendedData; }

    UPROPERTY(BlueprintAssignable)
    FOnFocusLost OnFocusLost;

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CustomSettings")
    bool bActive = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CustomSettings")
    ADMReceiver* MediapipeReceiver;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CustomSettings")
    EDMTrackingType TrackingType = EDMTrackingType::Default;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CustomSettings")
    FName SubjectName = LiveLinkName::IPhone;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CustomSettings", Meta = (ClampMin = 0.f, ClampMax = 3.f))
    float FocusLostTimeInSeconds = 1.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CustomSettings")
    FDMCalibrationSettings CalibrationSettings;

    virtual void SetMorphTarget(
        const EARFaceBlendShape& InDefaultType, const float DefaultValue, FName UpdateName, float UpdateValue);

    TArray<float> PreviousFrameValues;

private:
    void Initialize();

    bool bFocusLost = false;
    FDMLiveLinkExtendedData ExtendedData;

    ILiveLinkClient* LiveLinkClient;
    USkeletalMeshComponent* BodySkeletalMeshComponent;
    FTimerHandle FocusLostTimerHandle;

    UFUNCTION()
    void LiveLinkUpdated(float DeltaTime);

    UFUNCTION()
    void MediapipeUpdated(const FString& InMessage);

    void UpdateData(const TArray<float>& InRawFrameValues);

    void UpdateFocusLostTimerState(const bool bHasChanges, FTimerManager& InTimerManager);
    void FocusLostByTimer();
    void UpdateExtendedData(const TArray<float>& InFrameValues);
};

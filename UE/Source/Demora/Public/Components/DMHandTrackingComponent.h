// exp.dev free

#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include "Types/DMHandTrackingTypes.h"

#include "DMHandTrackingComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFocusLostLeft);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFocusLostRight);

class ADMReceiver;

UCLASS(ClassGroup = (DM), meta = (BlueprintSpawnableComponent, DisplayName = "HandTrackingComponent"))
class DEMORA_API UDMHandTrackingComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UDMHandTrackingComponent();

    UFUNCTION(BlueprintCallable)
    FDMHandTrackingData GetTrackingData() const { return TrackingData; }

    UPROPERTY(BlueprintAssignable)
    FOnFocusLostLeft OnFocusLostLeft;

    UPROPERTY(BlueprintAssignable)
    FOnFocusLostRight OnFocusLostRight;

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CustomSettings")
    bool bActive = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CustomSettings", Meta = (ClampMin = 0.016f, ClampMax = 1.f))
    float TickInSeconds = 0.016f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CustomSettings", Meta = (ClampMin = 0.1f, ClampMax = 1.f))
    float TickMultiply = 0.25f;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CustomSettings", Meta = (ClampMin = 0.f, ClampMax = 3.f))
    float FocusLostTimeInSeconds = 1.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CustomSettings")
    ADMReceiver* MediapipeReceiver;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CustomSettings")
    FDMHandTrackingSettings TrackingSettings;

private:
    void Initialize();

    FTimerHandle UpdateCurrentValuesTimerHandle;
    FTimerHandle FocusLostLeftTimerHandle;
    FTimerHandle FocusLostRightTimerHandle;

    const float ErrorTolerance = 0.01f;

    FDMHandTrackingData TrackingData;
    TArray<float> CurrentLeft;
    TArray<float> CurrentRight;
    TArray<float> TargetLeft;
    TArray<float> TargetRight;

    UFUNCTION()
    void UpdateTargetValues(const FString& InMessage);
    void UpdateCurrentValues();
    void UpdateFocusLostTimerState(
        const bool bHasChangesLeft, const bool bHasChangesRight, FTimerManager& InTimerManager);
    void FocusLostByTimerLeft();
    void FocusLostByTimerRight();
};

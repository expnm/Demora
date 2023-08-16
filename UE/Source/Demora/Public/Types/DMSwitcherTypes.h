#pragma once

#include "CoreMinimal.h"

#include "DMSwitcherTypes.generated.h"

UENUM(BlueprintType)
enum class EDMSwitchTarget : uint8
{
    Face,
    Hair,
    Hat,
    Skirt,
    Hands,
    Child,
    MAX
};

UENUM(BlueprintType)
enum class EDMAnimation : uint8
{
    Idle,
    Idle2,
    Jumping,
};

struct FDMTimerData
{
public:
    FDMTimerData() = default;
    FDMTimerData(const FTimerDelegate& InTimerMethod, const float MinValue = 0.f, const float MaxValue = 1.f)
        : TimerMethod(InTimerMethod), MinValue(MinValue), MaxValue(MaxValue)
    {
    }

    void SwitchIncrease() { bIncrease = !bIncrease; }
    bool IsIncrease() const { return bIncrease; }

    void StartTimer(FTimerManager& InTimerManager, float InRate)
    {
        if (!IsTimerActive(InTimerManager)) SetTimer(InTimerManager, InRate);
    }

    void StopTimer(FTimerManager& InTimerManager)
    {
        if (IsTimerActive(InTimerManager)) ClearTimer(InTimerManager);
    }

    float GetCurrentValue() const { return CurrentValue; }
    void AddToCurrentValue(const float InValue)
    {
        CurrentValue = FMath::Clamp(CurrentValue + InValue, MinValue, MaxValue);
    }
    bool IsCurrentValueMax() const { return FMath::IsNearlyEqual(CurrentValue, MaxValue); }
    bool IsCurrentValueMin() const { return FMath::IsNearlyEqual(CurrentValue, MinValue); }
    bool IsCurrentValueInTarget() const
    {
        return bIncrease && IsCurrentValueMax() || !bIncrease && IsCurrentValueMin();
    }

private:
    bool IsTimerActive(const FTimerManager& InTimerManager) const { return InTimerManager.IsTimerActive(TimerHandle); }
    void ClearTimer(FTimerManager& InTimerManager) { InTimerManager.ClearTimer(TimerHandle); }
    void SetTimer(FTimerManager& InTimerManager, float InRate)
    {
        InTimerManager.SetTimer(TimerHandle, TimerMethod, InRate, true);
    }

    FTimerHandle TimerHandle;
    FTimerDelegate TimerMethod;
    bool bIncrease = false;
    float CurrentValue = 0.f;
    float MinValue;
    float MaxValue;
};

USTRUCT(BlueprintType)
struct FDMSwitcherSettings
{
    GENERATED_USTRUCT_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (ClampMin = 0.016f, ClampMax = 1.f))
    float UpdateTickInSecond = 0.1f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (ClampMin = 0.1f, ClampMax = 10.f))
    float UpdateTimeInSecond = 3.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (ClampMin = 0.f, ClampMax = 100.f))
    float DissolveHandSphereRadius = 90.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (ClampMin = 0.1f, ClampMax = 2.f))
    float HandScaleMin = 0.75f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (ClampMin = 0.1f, ClampMax = 2.f))
    float HandScaleMax = 1.25f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (ClampMin = 0.1f, ClampMax = 2.f))
    float ChildBodyScale = 0.75f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (ClampMin = 0.1f, ClampMax = 2.f))
    float ChildHeadScale = 0.85f;
};

USTRUCT(BlueprintType)
struct FDMSwitcherExtendedData
{
    GENERATED_USTRUCT_BODY()

public:
    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
    float ChildBodyScale = 1.f;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
    float ChildHeadScale = 1.f;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
    float AngryFingersScale = 1.f;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
    float HandVisibility = 1.f;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
    int AnimationID = 0;
};

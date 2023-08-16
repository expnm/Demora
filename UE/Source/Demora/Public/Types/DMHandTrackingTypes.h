#pragma once

#include "CoreMinimal.h"

#include "DMHandTrackingTypes.generated.h"

UENUM(BlueprintType)
enum class EDMHandLandmark : uint8
{
    HandRoll,
    HandPitch,
    HandYaw,

    HandX,
    HandY,
    HandZ,

    IndexFinger,
    MiddleFinger,
    RingFinger,
    PinkyFinger,
    ThumbFinger,

    MAX
};

USTRUCT(BlueprintType)
struct FDMHandTrackingSettings
{
    GENERATED_USTRUCT_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bFlipTracking = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FVector MinLocationRight = FVector(-50.f, 0.f, 120.f);

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FVector MaxLocationRight = FVector(30.f, 30.f, 170.f);

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FRotator MinRotationRight = FRotator(-90.f, -30.f, -210.f);

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FRotator MaxRotationRight = FRotator(45.f, 90.f, -30.f);

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float MinFingerAngle = -15.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float MaxFingerAngle = 75.f;
};

USTRUCT(BlueprintType)
struct FDMHandTrackingData
{
    GENERATED_USTRUCT_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bLostLeft = true;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bLostRight = true;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FRotator HandRotationLeft = FRotator(0.f);

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FRotator HandRotationRight = FRotator(0.f);

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FVector HandLocationLeft = FVector(0.f);

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FVector HandLocationRight = FVector(0.f);

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    double IndexFingerLeft = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    double IndexFingerRight = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    double MiddleFingerLeft = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    double MiddleFingerRight = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    double RingFingerLeft = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    double RingFingerRight = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    double PinkyFingerLeft = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    double PinkyFingerRight = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    double ThumbFingerLeft = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    double ThumbFingerRight = 0.f;

    void Calculate(const bool bRight, const FDMHandTrackingSettings& InTrackingSettings, const FRotator& InHandRotator,
        const FVector& InHandLocation, const double IndexFinger, const double MiddleFinger, const double RingFinger,
        const double PinkyFinger, const double ThumbFinger)
    {
        CalculateHandRotator(bRight, InTrackingSettings, InHandRotator);
        CalculateLocation(bRight, InTrackingSettings, InHandLocation);

        if (bRight)
        {
            IndexFingerRight = CalculateFingerAngle(InTrackingSettings, IndexFinger);
            MiddleFingerRight = CalculateFingerAngle(InTrackingSettings, MiddleFinger);
            RingFingerRight = CalculateFingerAngle(InTrackingSettings, RingFinger);
            PinkyFingerRight = CalculateFingerAngle(InTrackingSettings, PinkyFinger);
            ThumbFingerRight = CalculateFingerAngle(InTrackingSettings, ThumbFinger);
        }
        else
        {
            IndexFingerLeft = CalculateFingerAngle(InTrackingSettings, IndexFinger);
            MiddleFingerLeft = CalculateFingerAngle(InTrackingSettings, MiddleFinger);
            RingFingerLeft = CalculateFingerAngle(InTrackingSettings, RingFinger);
            PinkyFingerLeft = CalculateFingerAngle(InTrackingSettings, PinkyFinger);
            ThumbFingerLeft = CalculateFingerAngle(InTrackingSettings, ThumbFinger);
        }
    }

private:
    void CalculateHandRotator(
        const bool bRight, const FDMHandTrackingSettings& InTrackingSettings, const FRotator& InHandRotator)
    {
        const TRange<double> InputRange = TRange<double>(-1.f, 1.f);
        const int SideMultiply = bRight ? 1.f : -1.f;

        double Roll = FMath::GetMappedRangeValueClamped(InputRange,
            TRange<double>(InTrackingSettings.MinRotationRight.Roll * SideMultiply,
                InTrackingSettings.MaxRotationRight.Roll * SideMultiply),
            InHandRotator.Roll);

        double Pitch = FMath::GetMappedRangeValueClamped(InputRange,
            TRange<double>(InTrackingSettings.MinRotationRight.Pitch, InTrackingSettings.MaxRotationRight.Pitch),
            InHandRotator.Pitch);

        double Yaw = FMath::GetMappedRangeValueClamped(InputRange,
            TRange<double>(InTrackingSettings.MinRotationRight.Yaw * SideMultiply,
                InTrackingSettings.MaxRotationRight.Yaw * SideMultiply),
            InHandRotator.Yaw);

        if (bRight)
        {
            HandRotationRight = FRotator(Pitch, Yaw, Roll);
        }
        else
        {
            HandRotationLeft = FRotator(Pitch, Yaw, Roll);
        }
    }

    void CalculateLocation(
        const bool bRight, const FDMHandTrackingSettings& InTrackingSettings, const FVector& InHandLocation)
    {
        const TRange<double> InputRange = TRange<double>(0.f, 1.f);
        const int SideMultiply = bRight ? 1.f : -1.f;

        double X = FMath::GetMappedRangeValueClamped(InputRange,
            TRange<double>(InTrackingSettings.MinLocationRight.X * SideMultiply,
                InTrackingSettings.MaxLocationRight.X * SideMultiply),
            InHandLocation.X);

        double Y = FMath::GetMappedRangeValueClamped(InputRange,
            TRange<double>(InTrackingSettings.MinLocationRight.Y, InTrackingSettings.MaxLocationRight.Y),
            InHandLocation.Y);

        double Z = FMath::GetMappedRangeValueClamped(InputRange,
            TRange<double>(InTrackingSettings.MinLocationRight.Z, InTrackingSettings.MaxLocationRight.Z),
            InHandLocation.Z);

        if (bRight)
        {
            HandLocationRight = FVector(X, Y, Z);
        }
        else
        {
            HandLocationLeft = FVector(X, Y, Z);
        }
    }

    double CalculateFingerAngle(const FDMHandTrackingSettings& InTrackingSettings, const double Finger)
    {
        const TRange<double> InputRange = TRange<double>(0.f, 1.f);

        double Angle = FMath::GetMappedRangeValueClamped(
            InputRange, TRange<double>(InTrackingSettings.MinFingerAngle, InTrackingSettings.MaxFingerAngle), Finger);

        return Angle;
    }
};

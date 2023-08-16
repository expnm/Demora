#pragma once

#include "ARTrackable.h"
#include "CoreMinimal.h"

#include "DMLiveLinkTypes.generated.h"

UENUM(BlueprintType)
enum class EDMTrackingType : uint8
{
    Default,
    Mediapipe,
};

USTRUCT(BlueprintType)
struct FDMLiveLinkExtendedData
{
    GENERATED_USTRUCT_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FRotator HeadRotation = FRotator(0.f);

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FRotator LeftEyeRotation = FRotator(0.f);

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FRotator RightEyeRotation = FRotator(0.f);

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bTongueOut = false;
};

USTRUCT(BlueprintType)
struct FDMCalibrationBlendShape
{
    GENERATED_USTRUCT_BODY()

public:
    FDMCalibrationBlendShape() = default;
    FDMCalibrationBlendShape(const EARFaceBlendShape& InDefaultType, const float Multiply = 1.f)
        : DefaultType(InDefaultType), Multiply(Multiply)
    {
        const FString BlendShapeName = StaticEnum<EARFaceBlendShape>()->GetNameStringByValue((int64)InDefaultType);
        CustomName = FName(BlendShapeName);
    }

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EARFaceBlendShape DefaultType = EARFaceBlendShape::TongueOut;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName CustomName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (ClampMin = 0.f, ClampMax = 2.f))
    float Multiply = 1.f;
};

ENUM_RANGE_BY_COUNT(EARFaceBlendShape, EARFaceBlendShape::MAX);

USTRUCT(BlueprintType)
struct FDMCalibrationSettings
{
    GENERATED_USTRUCT_BODY()

public:
    FDMCalibrationSettings()
    {
        for (const EARFaceBlendShape& BlendShape : TEnumRange<EARFaceBlendShape>())
        {
            BlendShapes.Emplace(BlendShape);
        }
    }

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bFlipTracking = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FRotator HeadAngleRotation = FRotator(45.f, 60.f, 90.f);

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FRotator HeadAngleOffset = FRotator(0.f);

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FRotator LeftEyeAngleRotation = FRotator(0.f, 30.f, 60.f);

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FRotator LeftEyeAngleOffset = FRotator(0.f);

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FRotator RightEyeAngleRotation = FRotator(0.f, 30.f, 60.f);

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FRotator RightEyeAngleOffset = FRotator(0.f);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (ClampMin = 0.f, ClampMax = 1.f))
    float TongueOutValue = 0.2f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<FDMCalibrationBlendShape> BlendShapes;
};

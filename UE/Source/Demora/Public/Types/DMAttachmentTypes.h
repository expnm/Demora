#pragma once

#include "CoreMinimal.h"
#include "GroomComponent.h"
#include "Types/DMStringLiterals.h"

#include "DMAttachmentTypes.generated.h"

class UActorComponent;

UENUM(BlueprintType)
enum class EDMAttachment : uint8
{
    Body,
    Brows,
    Hair,
    Hair2,
    Hat,
    Skirt,
    Bowtie4,
    DressFix,
    MAX
};

ENUM_RANGE_BY_COUNT(EDMAttachment, EDMAttachment::MAX);

enum class EDMBlendShape : uint8
{
    Mouth2,
    Child,
};

const TMap<EDMBlendShape, FName> DMBlendShapes = {
    {EDMBlendShape::Mouth2, BlendShapeName::Mouth2},
    {EDMBlendShape::Child, BlendShapeName::Child},
};

enum class EDMSocket : uint8
{
    ArmDissolveL,
    ArmDissolveR,
};

const TMap<EDMSocket, FName> DMSockets = {
    {EDMSocket::ArmDissolveL, SocketName::ArmDissolveL},
    {EDMSocket::ArmDissolveR, SocketName::ArmDissolveR},
};

USTRUCT(BlueprintType)
struct FDMAttachmentData
{
    GENERATED_USTRUCT_BODY()

public:
    FDMAttachmentData() = default;
    FDMAttachmentData(const EDMAttachment& InType) : Type(InType) {}

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EDMAttachment Type = EDMAttachment::Body;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString CustomName = TEXT("");

    void SetComponent(UActorComponent* InComponent) { Component = InComponent; }
    UActorComponent* GetComponent() const { return Component; }
    bool IsComponentAvailable() const { return Component ? true : false; }

    void SetVisibility(const bool InbVisibility) { bVisibility = InbVisibility; }
    void SwitchVisibility() { bVisibility = !bVisibility; }
    bool IsVisible() const { return bVisibility; }

    void UpdateSkeletalMeshComponentVisibility()
    {
        USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(Component);
        if (SkeletalMeshComponent)
        {
            SkeletalMeshComponent->SetVisibility(bVisibility);
            SkeletalMeshComponent->SetAnimationMode(
                bVisibility ? EAnimationMode::AnimationBlueprint : EAnimationMode::AnimationCustomMode);
        }
    }

    void UpdateGroomComponentVisibility(const bool bSetSimulation = false)
    {
        UGroomComponent* GroomComponent = Cast<UGroomComponent>(Component);
        if (GroomComponent)
        {
            GroomComponent->SetVisibility(bVisibility);
            if (bSetSimulation) GroomComponent->SetEnableSimulation(bVisibility);
        }
    }

    void SetMorphTarget(const EDMBlendShape& InBlendShape, float Value)
    {
        USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(Component);
        if (SkeletalMeshComponent)
        {
            const FName* ParamName = DMBlendShapes.Find(InBlendShape);
            check(ParamName);
            SkeletalMeshComponent->SetMorphTarget(*ParamName, Value);
        }
    }

    FVector GetSocketLocation(const EDMSocket& InSocket)
    {
        USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(Component);
        if (SkeletalMeshComponent)
        {
            const FName* ParamName = DMSockets.Find(InSocket);
            check(ParamName);
            return SkeletalMeshComponent->GetSocketLocation(*ParamName);
        }
        return FVector();
    }

private:
    UActorComponent* Component = nullptr;
    bool bVisibility = true;
};

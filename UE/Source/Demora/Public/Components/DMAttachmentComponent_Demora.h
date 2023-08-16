// exp.dev free

#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include "Types/DMAttachmentTypes.h"

#include "DMAttachmentComponent_Demora.generated.h"

UCLASS(ClassGroup = (DM), meta = (BlueprintSpawnableComponent, DisplayName = "AttachmentComponentDemora"))
class DEMORA_API UDMAttachmentComponent_Demora : public UActorComponent
{
    GENERATED_BODY()

public:
    UDMAttachmentComponent_Demora();

    bool IsVisible(const EDMAttachment& InAttachment) const;
    void SwitchVisibility(const EDMAttachment& InAttachment);
    void SwitchVisibility(const TArray<EDMAttachment>& InAttachments);

    void SetMorphTarget(const EDMAttachment& InAttachment, const EDMBlendShape& InBlendShape, float Value);
    FVector GetSocketLocation(const EDMAttachment& InAttachment, const EDMSocket& InSocket);

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CustomSettings")
    bool bActive = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CustomSettings")
    TArray<FDMAttachmentData> AttachmentData;

private:
    void Initialize();
    void UpdateVisibility(const TArray<EDMAttachment>& InAttachments);
};

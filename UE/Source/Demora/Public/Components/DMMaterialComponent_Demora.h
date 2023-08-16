// exp.dev free

#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include "Types/DMMaterialTypes.h"

#include "DMMaterialComponent_Demora.generated.h"

UCLASS(ClassGroup = (DM), meta = (BlueprintSpawnableComponent, DisplayName = "MaterialComponentDemora"))
class DEMORA_API UDMMaterialComponent_Demora : public UActorComponent
{
    GENERATED_BODY()

public:
    UDMMaterialComponent_Demora();

    void SetParameter(const EDMMaterialParameter& InMaterialParameter, const float ScalarValue);
    void SetParameter(const EDMMaterialParameter& InMaterialParameter, const FVector4& InVectorValue);

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CustomSettings")
    bool bActive = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CustomSettings")
    TArray<FDMMaterialAttachmentData> AttachmentData;

private:
    void Initialize();
};

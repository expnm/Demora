// exp.dev free

#pragma once

#include "Components/DMLiveLinkComponent.h"
#include "CoreMinimal.h"

#include "DMLiveLinkComponent_Demora.generated.h"

class UDMSwitcherComponent_Demora;

UCLASS(ClassGroup = (DM), meta = (BlueprintSpawnableComponent, DisplayName = "LiveLinkComponentDemora"))
class DEMORA_API UDMLiveLinkComponent_Demora : public UDMLiveLinkComponent
{
    GENERATED_BODY()

public:
    bool IsAutoSwitchFace() const { return bAutoSwitchFace; }

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CustomSettings")
    bool bAutoSwitchFace = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CustomSettings")
    FDMCalibrationSettings SecondFaceCalibrationSettings;

    virtual void SetMorphTarget(
        const EARFaceBlendShape& InDefaultType, const float DefaultValue, FName UpdateName, float UpdateValue) override;

private:
    UDMSwitcherComponent_Demora* SwitcherComponent;
};

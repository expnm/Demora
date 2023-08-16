// exp.dev free

#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include "Types/DMControlPanelTypes.h"
#include "Types/DMSwitcherTypes.h"

#include "DMControlPanelComponent_Demora.generated.h"

class ADMReceiver;
class UDMSwitcherComponent_Demora;

UCLASS(ClassGroup = (DM), meta = (BlueprintSpawnableComponent, DisplayName = "ControlPanelComponentDemora"))
class DEMORA_API UDMControlPanelComponent_Demora : public UActorComponent
{
    GENERATED_BODY()

public:
    UDMControlPanelComponent_Demora();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CustomSettings")
    bool bActive = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CustomSettings")
    ADMReceiver* ControlPanelReceiver;

private:
    void Initialize();

    UFUNCTION()
    void ReceiverUpdated(const FString& InMessage);

    UDMSwitcherComponent_Demora* SwitcherComponent;

    TMap<EDMControlPanelEvent, EDMSwitchTarget> SwitcherTargets = {
        {EDMControlPanelEvent::SwitchFace, EDMSwitchTarget::Face},
        {EDMControlPanelEvent::SwitchHair, EDMSwitchTarget::Hair},
        {EDMControlPanelEvent::HideHat, EDMSwitchTarget::Hat},
        {EDMControlPanelEvent::HideSkirt, EDMSwitchTarget::Skirt},
        {EDMControlPanelEvent::HideHands, EDMSwitchTarget::Hands},
        {EDMControlPanelEvent::Child, EDMSwitchTarget::Child},
    };

    TMap<EDMControlPanelEvent, EDMAnimation> SwitcherAnimations = {
        {EDMControlPanelEvent::Animation1, EDMAnimation::Idle},
        {EDMControlPanelEvent::Animation2, EDMAnimation::Idle2},
        {EDMControlPanelEvent::Animation3, EDMAnimation::Jumping},
    };
};

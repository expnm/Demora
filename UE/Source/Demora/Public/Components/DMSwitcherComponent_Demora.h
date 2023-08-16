// exp.dev free

#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include "Types/DMStringLiterals.h"
#include "Types/DMSwitcherTypes.h"

#include "DMSwitcherComponent_Demora.generated.h"

class UDMAttachmentComponent_Demora;
class UDMMaterialComponent_Demora;
class UDMLiveLinkComponent_Demora;

UCLASS(ClassGroup = (DM), meta = (BlueprintSpawnableComponent, DisplayName = "SwitcherComponentDemora"))
class DEMORA_API UDMSwitcherComponent_Demora : public UActorComponent
{
    GENERATED_BODY()

public:
    UDMSwitcherComponent_Demora();

    UFUNCTION(BlueprintCallable)
    void Switch(const EDMSwitchTarget& InTarget);

    UFUNCTION(BlueprintCallable)
    void SetAnimation(const EDMAnimation& InAnimation) { ExtendedData.AnimationID = (int)InAnimation; };

    UFUNCTION(BlueprintCallable)
    FDMSwitcherExtendedData GetExtendedData() const { return ExtendedData; };

    void SetFaceValue(const float Value);

    float GetCurrentValue(const EDMSwitchTarget& InTarget) const
    {
        if (SwitchTargetTimerData.Find(InTarget)) return SwitchTargetTimerData.Find(InTarget)->GetCurrentValue();
        return 0.f;
    }

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CustomSettings")
    bool bActive = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CustomSettings")
    FDMSwitcherSettings SwitcherSettings;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CustomSettings")
    UDMAttachmentComponent_Demora* AttachmentComponent;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CustomSettings")
    UDMMaterialComponent_Demora* MaterialComponent;


private:
    UDMLiveLinkComponent_Demora* LiveLinkComponent;
    FDMSwitcherExtendedData ExtendedData;

    FTimerHandle InitTimerHandle;

    TRange<float> DefaultRange = TRange<float>(0.f, 1.f);
    float UpdateStepValue;

    void Initialize();

    void SwitchFace();
    void SwitchHair();
    void SwitchHat();
    void SwitchSkirt();
    void SwitchHands();
    void SwitchChild();

    TMap<EDMSwitchTarget, FDMTimerData> SwitchTargetTimerData = {
        {EDMSwitchTarget::Face, {FTimerDelegate::CreateUObject(this, &UDMSwitcherComponent_Demora::SwitchFace)}},
        {EDMSwitchTarget::Hair, {FTimerDelegate::CreateUObject(this, &UDMSwitcherComponent_Demora::SwitchHair)}},
        {EDMSwitchTarget::Hat, {FTimerDelegate::CreateUObject(this, &UDMSwitcherComponent_Demora::SwitchHat)}},
        {EDMSwitchTarget::Skirt, {FTimerDelegate::CreateUObject(this, &UDMSwitcherComponent_Demora::SwitchSkirt)}},
        {EDMSwitchTarget::Hands, {FTimerDelegate::CreateUObject(this, &UDMSwitcherComponent_Demora::SwitchHands)}},
        {EDMSwitchTarget::Child, {FTimerDelegate::CreateUObject(this, &UDMSwitcherComponent_Demora::SwitchChild)}},
    };
};

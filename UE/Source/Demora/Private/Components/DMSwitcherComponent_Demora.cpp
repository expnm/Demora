// exp.dev free

#include "Components/DMSwitcherComponent_Demora.h"
#include "Components/DMAttachmentComponent_Demora.h"
#include "Components/DMLiveLinkComponent_Demora.h"
#include "Components/DMMaterialComponent_Demora.h"

UDMSwitcherComponent_Demora::UDMSwitcherComponent_Demora()
{
    AttachmentComponent = CreateDefaultSubobject<UDMAttachmentComponent_Demora>(ComponentName::AttachmentComponent);
    MaterialComponent = CreateDefaultSubobject<UDMMaterialComponent_Demora>(ComponentName::MaterialComponent);

#if WITH_EDITOR
    if (GEditor)
    {
        GEditor->GetTimerManager()->SetTimerForNextTick(this, &UDMSwitcherComponent_Demora::Initialize);
        // GEditor->GetTimerManager()->SetTimer(InitTimerHandle, this, &UDMSwitcherComponent_Demora::Initialize, 2.f);
    }
#endif // WITH_EDITOR
}

void UDMSwitcherComponent_Demora::BeginPlay()
{
    Super::BeginPlay();

#if !WITH_EDITOR
    Initialize();
#endif // !WITH_EDITOR
}

void UDMSwitcherComponent_Demora::Initialize()
{
    if (!bActive) return;

    const AActor* Owner = GetOwner();
    if (Owner)
    {
        LiveLinkComponent = Owner->FindComponentByClass<UDMLiveLinkComponent_Demora>();
    }

    check(SwitcherSettings.UpdateTimeInSecond > 0.f);
    UpdateStepValue = SwitcherSettings.UpdateTickInSecond / SwitcherSettings.UpdateTimeInSecond;
}

void UDMSwitcherComponent_Demora::Switch(const EDMSwitchTarget& InTarget)
{
    FDMTimerData* Data = SwitchTargetTimerData.Find(InTarget);
    if (!Data) return;

    Data->SwitchIncrease();

#if WITH_EDITOR
    Data->StartTimer(*GEditor->GetTimerManager(), SwitcherSettings.UpdateTickInSecond);
#else
    Data->StartTimer(GetWorld()->GetTimerManager(), SwitcherSettings.UpdateTickInSecond);
#endif // WITH_EDITOR
}

void UDMSwitcherComponent_Demora::SwitchFace()
{
    FDMTimerData* Data = SwitchTargetTimerData.Find(EDMSwitchTarget::Face);
    if (!Data) return;

    if (LiveLinkComponent && LiveLinkComponent->IsAutoSwitchFace() || Data->IsCurrentValueInTarget())
    {
#if WITH_EDITOR
        Data->StopTimer(*GEditor->GetTimerManager());
#else
        Data->StopTimer(GetWorld()->GetTimerManager());
#endif // WITH_EDITOR
    }

    Data->AddToCurrentValue(UpdateStepValue * (Data->IsIncrease() ? 1.f : -1.f));
    SetFaceValue(Data->GetCurrentValue());
}

void UDMSwitcherComponent_Demora::SetFaceValue(const float Value)
{
    MaterialComponent->SetParameter(EDMMaterialParameter::Eye2, Value);
    AttachmentComponent->SetMorphTarget(EDMAttachment::Body, EDMBlendShape::Mouth2, Value);

    const float CurrentMappedValue = FMath::GetMappedRangeValueClamped(
        DefaultRange, TRange<float>(SwitcherSettings.HandScaleMin, SwitcherSettings.HandScaleMax), Value);
    ExtendedData.AngryFingersScale = CurrentMappedValue;
}

void UDMSwitcherComponent_Demora::SwitchHair()
{
    FDMTimerData* Data = SwitchTargetTimerData.Find(EDMSwitchTarget::Hair);
    if (!Data) return;

    AttachmentComponent->SwitchVisibility(EDMAttachment::Hair);

    const bool bShowTails =
        AttachmentComponent->IsVisible(EDMAttachment::Hair2) && !AttachmentComponent->IsVisible(EDMAttachment::Hat);
    MaterialComponent->SetParameter(EDMMaterialParameter::ShowTails, (bShowTails ? 1.f : 0.f));

#if WITH_EDITOR
    Data->StopTimer(*GEditor->GetTimerManager());
#else
    Data->StopTimer(GetWorld()->GetTimerManager());
#endif // WITH_EDITOR
}

void UDMSwitcherComponent_Demora::SwitchHat()
{
    FDMTimerData* Data = SwitchTargetTimerData.Find(EDMSwitchTarget::Hat);
    if (!Data) return;

    AttachmentComponent->SwitchVisibility(EDMAttachment::Hat);

    const bool bShowTails =
        AttachmentComponent->IsVisible(EDMAttachment::Hair2) && !AttachmentComponent->IsVisible(EDMAttachment::Hat);
    MaterialComponent->SetParameter(EDMMaterialParameter::ShowTails, (bShowTails ? 1.f : 0.f));

#if WITH_EDITOR
    Data->StopTimer(*GEditor->GetTimerManager());
#else
    Data->StopTimer(GetWorld()->GetTimerManager());
#endif // WITH_EDITOR
}

void UDMSwitcherComponent_Demora::SwitchSkirt()
{
    FDMTimerData* Data = SwitchTargetTimerData.Find(EDMSwitchTarget::Skirt);
    if (!Data) return;

    AttachmentComponent->SwitchVisibility(EDMAttachment::Skirt);

#if WITH_EDITOR
    Data->StopTimer(*GEditor->GetTimerManager());
#else
    Data->StopTimer(GetWorld()->GetTimerManager());
#endif // WITH_EDITOR
}

void UDMSwitcherComponent_Demora::SwitchHands()
{
    FDMTimerData* Data = SwitchTargetTimerData.Find(EDMSwitchTarget::Hands);
    if (!Data) return;

    if (Data->IsCurrentValueInTarget())
    {
#if WITH_EDITOR
        Data->StopTimer(*GEditor->GetTimerManager());
#else
        Data->StopTimer(GetWorld()->GetTimerManager());
#endif // WITH_EDITOR
    }

    Data->AddToCurrentValue(UpdateStepValue * (Data->IsIncrease() ? 1.f : -1.f));

    const FVector DissolveVectorL =
        AttachmentComponent->GetSocketLocation(EDMAttachment::Body, EDMSocket::ArmDissolveL);
    const FVector DissolveVectorR =
        AttachmentComponent->GetSocketLocation(EDMAttachment::Body, EDMSocket::ArmDissolveR);

    MaterialComponent->SetParameter(EDMMaterialParameter::SpherePos1, DissolveVectorL);
    MaterialComponent->SetParameter(EDMMaterialParameter::SpherePos2, DissolveVectorR);

    const float CurrentMappedValue = FMath::GetMappedRangeValueClamped(
        DefaultRange, TRange<float>(0.f, SwitcherSettings.DissolveHandSphereRadius), Data->GetCurrentValue());

    MaterialComponent->SetParameter(EDMMaterialParameter::SphereRadius1, CurrentMappedValue);
    MaterialComponent->SetParameter(EDMMaterialParameter::SphereRadius2, CurrentMappedValue);

    ExtendedData.HandVisibility = 1.f - Data->GetCurrentValue();
}

void UDMSwitcherComponent_Demora::SwitchChild()
{
    FDMTimerData* Data = SwitchTargetTimerData.Find(EDMSwitchTarget::Child);
    if (!Data) return;

    if (Data->IsCurrentValueInTarget())
    {
#if WITH_EDITOR
        Data->StopTimer(*GEditor->GetTimerManager());
#else
        Data->StopTimer(GetWorld()->GetTimerManager());
#endif // WITH_EDITOR
    }

    Data->AddToCurrentValue(UpdateStepValue * (Data->IsIncrease() ? 1.f : -1.f));

    AttachmentComponent->SetMorphTarget(EDMAttachment::Body, EDMBlendShape::Child, Data->GetCurrentValue());
    AttachmentComponent->SetMorphTarget(EDMAttachment::DressFix, EDMBlendShape::Child, Data->GetCurrentValue());

    const float CurrentBodyMappedValue = FMath::GetMappedRangeValueClamped(
        DefaultRange, TRange<float>(1.f, SwitcherSettings.ChildBodyScale), Data->GetCurrentValue());
    const float CurrentHeadMappedValue = FMath::GetMappedRangeValueClamped(
        DefaultRange, TRange<float>(1.f, SwitcherSettings.ChildHeadScale), Data->GetCurrentValue());

    ExtendedData.ChildBodyScale = CurrentBodyMappedValue;
    ExtendedData.ChildHeadScale = CurrentHeadMappedValue;
}

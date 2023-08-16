// exp.dev free

#include "Components/DMControlPanelComponent_Demora.h"
#include "Components/DMSwitcherComponent_Demora.h"
#include "DMReceiver.h"

DEFINE_LOG_CATEGORY_STATIC(LogDMControlPanelComponent_Demora, All, All);

UDMControlPanelComponent_Demora::UDMControlPanelComponent_Demora()
{
#if WITH_EDITOR
    if (GEditor)
    {
        GEditor->GetTimerManager()->SetTimerForNextTick(this, &UDMControlPanelComponent_Demora::Initialize);
    }
#endif // WITH_EDITOR
}

void UDMControlPanelComponent_Demora::BeginPlay()
{
    Super::BeginPlay();

#if !WITH_EDITOR
    Initialize();
#endif // !WITH_EDITOR
}

void UDMControlPanelComponent_Demora::Initialize()
{
    if (!bActive) return;

    if (ControlPanelReceiver && ControlPanelReceiver->IsActive())
    {
        ControlPanelReceiver->Updated.AddDynamic(this, &UDMControlPanelComponent_Demora::ReceiverUpdated);
    }
}

void UDMControlPanelComponent_Demora::ReceiverUpdated(const FString& InMessage)
{
    if (!SwitcherComponent)
    {
        const AActor* Owner = GetOwner();
        if (Owner)
        {
            SwitcherComponent = Owner->FindComponentByClass<UDMSwitcherComponent_Demora>();
        }

        if (!SwitcherComponent)
        {
            UE_LOG(LogDMControlPanelComponent_Demora, Error, TEXT("SwitcherComponent is empty"));
            return;
        }
    }

    const EDMControlPanelEvent ControlPanelEvent = EDMControlPanelEvent((uint8)(FCString::Atof(*InMessage)));

    switch (ControlPanelEvent)
    {
        case EDMControlPanelEvent::SwitchFace:
        case EDMControlPanelEvent::SwitchHair:
        case EDMControlPanelEvent::HideHat:
        case EDMControlPanelEvent::HideSkirt:
        case EDMControlPanelEvent::HideHands:
        case EDMControlPanelEvent::Child:
        {
            AsyncTask(ENamedThreads::GameThread,
                [this, ControlPanelEvent]() { SwitcherComponent->Switch(*SwitcherTargets.Find(ControlPanelEvent)); });
        }
        break;

        case EDMControlPanelEvent::Animation1:
        case EDMControlPanelEvent::Animation2:
        case EDMControlPanelEvent::Animation3:
        {
            SwitcherComponent->SetAnimation(*SwitcherAnimations.Find(ControlPanelEvent));
        }
        break;
    }
}

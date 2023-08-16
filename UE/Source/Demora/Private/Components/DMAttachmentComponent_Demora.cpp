// exp.dev free

#include "Components/DMAttachmentComponent_Demora.h"
#include "GroomComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogDMAttachmentComponent_Demora, All, All);

UDMAttachmentComponent_Demora::UDMAttachmentComponent_Demora()
{
    for (const EDMAttachment& Attachment : TEnumRange<EDMAttachment>())
    {
        AttachmentData.Emplace(Attachment);

        if (Attachment == EDMAttachment::Hair)
        {
            AttachmentData.Last().SwitchVisibility();
        }
    }

#if WITH_EDITOR
    if (GEditor)
    {
        GEditor->GetTimerManager()->SetTimerForNextTick(this, &UDMAttachmentComponent_Demora::Initialize);
    }
#endif // WITH_EDITOR
}

void UDMAttachmentComponent_Demora::BeginPlay()
{
    Super::BeginPlay();

#if !WITH_EDITOR
    Initialize();
#endif // !WITH_EDITOR
}

void UDMAttachmentComponent_Demora::Initialize()
{
    if (!bActive) return;

    const AActor* Owner = GetOwner();
    if (!Owner) return;

    for (UActorComponent* Component : Owner->GetComponents())
    {
        const FString& ComponentName = Component->GetName();
        for (int i = 0; i < AttachmentData.Num(); ++i)
        {
            // если кастомное имя заполнено И НЕ равно имени текущей компоненты
            if (!AttachmentData[i].CustomName.IsEmpty() && AttachmentData[i].CustomName != ComponentName) continue;

            // если название вложения из перечисления НЕ равно имени текущей компоненты
            const FString AttachmentName =
                StaticEnum<EDMAttachment>()->GetNameStringByValue((int64)AttachmentData[i].Type);
            if (AttachmentName != ComponentName) continue;

            AttachmentData[i].SetComponent(Component);
        }
    }

    for (const FDMAttachmentData& Data :
        AttachmentData.FilterByPredicate([](const FDMAttachmentData& Data) { return !Data.IsComponentAvailable(); }))
    {
        const FString AttachmentName = StaticEnum<EDMAttachment>()->GetNameStringByValue((int64)Data.Type);
        UE_LOG(LogDMAttachmentComponent_Demora, Warning, TEXT("%s is not found"), *AttachmentName);
    }

    // установить разрешение на редактирование настроек волос
    FDMAttachmentData* HairData =
        AttachmentData.FindByPredicate([](const FDMAttachmentData& Data) { return Data.Type == EDMAttachment::Hair; });
    if (HairData && HairData->IsComponentAvailable())
    {
        UGroomComponent* HairGroomComponent = Cast<UGroomComponent>(HairData->GetComponent());
        if (HairGroomComponent)
        {
            HairGroomComponent->SimulationSettings.bOverrideSettings = true;
        }
        else
        {
            UE_LOG(LogDMAttachmentComponent_Demora, Error, TEXT("HairGroomComponent cast failed"));
        }
    }

#if WITH_EDITOR
    // включить анимацию
    for (FDMAttachmentData& Data :
        AttachmentData.FilterByPredicate([](const FDMAttachmentData& Data) { return Data.IsComponentAvailable(); }))
    {
        USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(Data.GetComponent());
        if (SkeletalMeshComponent) SkeletalMeshComponent->SetUpdateAnimationInEditor(true);
    }

    // включить симуляцию ткани
    FDMAttachmentData* BodyData =
        AttachmentData.FindByPredicate([](const FDMAttachmentData& Data) { return Data.Type == EDMAttachment::Body; });
    if (BodyData)
    {
        USkeletalMeshComponent* BodySkeletalMeshComponent = Cast<USkeletalMeshComponent>(BodyData->GetComponent());
        check(BodySkeletalMeshComponent);
        BodySkeletalMeshComponent->SetUpdateClothInEditor(true);
        BodySkeletalMeshComponent->RecreateClothingActors();
    }
#endif // !WITH_EDITOR
}

bool UDMAttachmentComponent_Demora::IsVisible(const EDMAttachment& InAttachment) const
{
    if (AttachmentData.IsEmpty()) return false;

    const FDMAttachmentData* Data = AttachmentData.FindByPredicate(
        [InAttachment](const FDMAttachmentData& Data) { return Data.Type == InAttachment; });

    if (!Data) return false;
    return Data->IsVisible();
}

void UDMAttachmentComponent_Demora::SwitchVisibility(const EDMAttachment& InAttachment)
{
    if (AttachmentData.IsEmpty()) return;

    FDMAttachmentData* Data = AttachmentData.FindByPredicate(
        [InAttachment](const FDMAttachmentData& Data) { return Data.Type == InAttachment; });
    if (!Data) return;

    Data->SwitchVisibility();
    UpdateVisibility({InAttachment});
}

void UDMAttachmentComponent_Demora::SwitchVisibility(const TArray<EDMAttachment>& InAttachments)
{
    if (AttachmentData.IsEmpty() || InAttachments.IsEmpty()) return;

    for (const EDMAttachment& Attachment : InAttachments)
    {
        FDMAttachmentData* CurrentData = AttachmentData.FindByPredicate(
            [Attachment](const FDMAttachmentData& Data) { return Data.Type == Attachment; });
        if (!CurrentData) continue;

        CurrentData->SwitchVisibility();
    }

    UpdateVisibility(InAttachments);
}

void UDMAttachmentComponent_Demora::UpdateVisibility(const TArray<EDMAttachment>& InAttachments)
{
    for (const EDMAttachment& Attachment : InAttachments)
    {
        FDMAttachmentData* CurrentData = AttachmentData.FindByPredicate(
            [Attachment](const FDMAttachmentData& Data) { return Data.Type == Attachment; });
        if (!CurrentData || !CurrentData->IsComponentAvailable()) continue;

        switch (Attachment)
        {
            case EDMAttachment::Brows:
            {
                CurrentData->UpdateGroomComponentVisibility(false);
            }
            break;

            case EDMAttachment::Hair:
            {
                CurrentData->UpdateGroomComponentVisibility(true);

                const FDMAttachmentData* Hair2Data = AttachmentData.FindByPredicate(
                    [](const FDMAttachmentData& Data) { return Data.Type == EDMAttachment::Hair2; });

                if (CurrentData->IsVisible() == Hair2Data->IsVisible())
                {
                    SwitchVisibility(EDMAttachment::Hair2);
                }
                else
                {
                    UpdateVisibility({EDMAttachment::Bowtie4});
                }
            }
            break;

            case EDMAttachment::Hair2:
            {
                CurrentData->UpdateSkeletalMeshComponentVisibility();

                const FDMAttachmentData* HairData = AttachmentData.FindByPredicate(
                    [](const FDMAttachmentData& Data) { return Data.Type == EDMAttachment::Hair; });

                if (CurrentData->IsVisible() == HairData->IsVisible())
                {
                    SwitchVisibility(EDMAttachment::Hair);
                }
                else
                {
                    UpdateVisibility({EDMAttachment::Bowtie4});
                }
            }
            break;

            case EDMAttachment::Bowtie4:
            {
                CurrentData->UpdateSkeletalMeshComponentVisibility();

                FDMAttachmentData* Bowtie4Data = AttachmentData.FindByPredicate(
                    [](const FDMAttachmentData& Data) { return Data.Type == EDMAttachment::Bowtie4; });

                const FDMAttachmentData* HairData = AttachmentData.FindByPredicate(
                    [](const FDMAttachmentData& Data) { return Data.Type == EDMAttachment::Hair; });

                const FDMAttachmentData* HatData = AttachmentData.FindByPredicate(
                    [](const FDMAttachmentData& Data) { return Data.Type == EDMAttachment::Hat; });

                const bool bCurrentVisibility = HairData->IsVisible() && !HatData->IsVisible();
                if (bCurrentVisibility == Bowtie4Data->IsVisible()) break;

                Bowtie4Data->SetVisibility(bCurrentVisibility);
                UpdateVisibility({EDMAttachment::Bowtie4});
            }
            break;

            case EDMAttachment::Hat:
            {
                CurrentData->UpdateSkeletalMeshComponentVisibility();
                UpdateVisibility({EDMAttachment::Bowtie4});
            }
            break;

            case EDMAttachment::Skirt:
            {
                CurrentData->UpdateSkeletalMeshComponentVisibility();
            }
            break;

            default:
            {
                UE_LOG(LogDMAttachmentComponent_Demora, Warning, TEXT("Out of range: %d"), Attachment);
            }
            break;
        }
    }
}

void UDMAttachmentComponent_Demora::SetMorphTarget(
    const EDMAttachment& InAttachment, const EDMBlendShape& InBlendShape, float Value)
{
    if (AttachmentData.IsEmpty()) return;

    FDMAttachmentData* Data = AttachmentData.FindByPredicate(
        [InAttachment](const FDMAttachmentData& Data) { return Data.Type == InAttachment; });
    if (!Data || !Data->IsComponentAvailable()) return;

    Data->SetMorphTarget(InBlendShape, Value);
}

FVector UDMAttachmentComponent_Demora::GetSocketLocation(const EDMAttachment& InAttachment, const EDMSocket& InSocket)
{
    if (AttachmentData.IsEmpty()) return FVector();

    FDMAttachmentData* Data = AttachmentData.FindByPredicate(
        [InAttachment](const FDMAttachmentData& Data) { return Data.Type == InAttachment; });
    if (!Data || !Data->IsComponentAvailable()) return FVector();

    return Data->GetSocketLocation(InSocket);
}

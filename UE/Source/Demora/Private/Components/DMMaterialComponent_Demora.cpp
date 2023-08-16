// exp.dev free

#include "Components/DMMaterialComponent_Demora.h"
#include "Materials/MaterialInstanceDynamic.h"

DEFINE_LOG_CATEGORY_STATIC(LogDMMaterialComponent_Demora, All, All);

UDMMaterialComponent_Demora::UDMMaterialComponent_Demora()
{
#if WITH_EDITOR
    if (GEditor)
    {
        GEditor->GetTimerManager()->SetTimerForNextTick(this, &UDMMaterialComponent_Demora::Initialize);
    }
#endif // WITH_EDITOR}
}

void UDMMaterialComponent_Demora::BeginPlay()
{
    Super::BeginPlay();
    
#if !WITH_EDITOR
    Initialize();
#endif // !WITH_EDITOR
}

void UDMMaterialComponent_Demora::Initialize()
{
    if (!bActive) return;

    if (!AttachmentData.Num())
    {
        UE_LOG(LogDMMaterialComponent_Demora, Error, TEXT("Fill material component!"));
        return;
    }

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

            UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(Component);
            if (!PrimitiveComponent) continue;

            AttachmentData[i].SetMaterial(PrimitiveComponent->CreateDynamicMaterialInstance(
                AttachmentData[i].MaterialSocketID, AttachmentData[i].SourceMaterial));

            PrimitiveComponent->SetMaterial(AttachmentData[i].MaterialSocketID, AttachmentData[i].GetMaterial());
        }
    }

    for (const FDMMaterialAttachmentData& Data : AttachmentData.FilterByPredicate(
             [](const FDMMaterialAttachmentData& Data) { return !Data.IsMaterialAvailable(); }))
    {
        UE_LOG(LogDMMaterialComponent_Demora, Error, TEXT("%s[%d] material is not created"),
            *UEnum::GetValueAsString(Data.Type), Data.MaterialSocketID);
    }
    
    SetParameter(EDMMaterialParameter::HideSimulated, 1.f);
    SetParameter(EDMMaterialParameter::ShowTails, 0.f);
}

void UDMMaterialComponent_Demora::SetParameter(const EDMMaterialParameter& InMaterialParameter, const float ScalarValue)
{
    if (AttachmentData.IsEmpty()) return;

    const FName* ParamName = DMMaterialParameters.Find(InMaterialParameter);
    check(ParamName);

    switch (InMaterialParameter)
    {
        case EDMMaterialParameter::Eye2:
        case EDMMaterialParameter::SphereRadius1:
        case EDMMaterialParameter::SphereRadius2:
        case EDMMaterialParameter::ShowTails:
        {
            for (const FDMMaterialAttachmentData& Data : AttachmentData.FilterByPredicate(
                     [](const FDMMaterialAttachmentData& Data) { return Data.IsMaterialAvailable(); }))
            {
                Data.GetMaterial()->SetScalarParameterValue(*ParamName, ScalarValue);
            }
        }
        break;

        case EDMMaterialParameter::HideSimulated:
        {
            const FDMMaterialAttachmentData* DressAttachmentData =
                AttachmentData.FindByPredicate([](const FDMMaterialAttachmentData& Data)
                    { return Data.Type == EDMAttachment::Body && Data.MaterialSocketID == DressMaterialSocketID; });

            const FDMMaterialAttachmentData* DressFixAttachmentData = AttachmentData.FindByPredicate(
                [](const FDMMaterialAttachmentData& Data) { return Data.Type == EDMAttachment::DressFix; });

            if ((DressAttachmentData && DressAttachmentData->IsMaterialAvailable()) &&
                (DressFixAttachmentData && DressFixAttachmentData->IsMaterialAvailable()))
            {
                DressAttachmentData->GetMaterial()->SetScalarParameterValue(*ParamName, 0.f);
                DressFixAttachmentData->GetMaterial()->SetScalarParameterValue(*ParamName, 1.f);
            }
        }
        break;
    }
}

void UDMMaterialComponent_Demora::SetParameter(
    const EDMMaterialParameter& InMaterialParameter, const FVector4& InVectorValue)
{
    if (AttachmentData.IsEmpty()) return;

    const FName* ParamName = DMMaterialParameters.Find(InMaterialParameter);
    check(ParamName);

    switch (InMaterialParameter)
    {
        case EDMMaterialParameter::SpherePos1:
        case EDMMaterialParameter::SpherePos2:
        {
            for (const FDMMaterialAttachmentData& Data : AttachmentData.FilterByPredicate(
                     [](const FDMMaterialAttachmentData& Data) { return Data.IsMaterialAvailable(); }))
            {
                Data.GetMaterial()->SetVectorParameterValue(*ParamName, InVectorValue);
            }
        }
        break;
    }
}

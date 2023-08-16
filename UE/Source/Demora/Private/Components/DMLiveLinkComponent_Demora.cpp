// exp.dev free

#include "Components/DMLiveLinkComponent_Demora.h"
#include "Components/DMSwitcherComponent_Demora.h"

void UDMLiveLinkComponent_Demora::SetMorphTarget(
    const EARFaceBlendShape& InDefaultType, const float DefaultValue, FName UpdateName, float UpdateValue)
{
    if (!PreviousFrameValues.IsEmpty())
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
                Super::SetMorphTarget(InDefaultType, DefaultValue, UpdateName, UpdateValue);
                return;
            }
        }

        const FDMCalibrationBlendShape* CalibrationBlendShape =
            SecondFaceCalibrationSettings.BlendShapes.FindByPredicate(
                [InDefaultType](const FDMCalibrationBlendShape& CalibrationBlendShape)
                { return CalibrationBlendShape.DefaultType == InDefaultType; });

        if (CalibrationBlendShape)
        {
            const float CalibrationMultiply = CalibrationBlendShape ? CalibrationBlendShape->Multiply : 1.f;
            float SecondValue = FMath::Clamp(DefaultValue * CalibrationMultiply, -1.f, 1.f);

            if (bAutoSwitchFace)
            {
                const float SmilingMultiply = (PreviousFrameValues[(int)EARFaceBlendShape::MouthSmileLeft] +
                                                  PreviousFrameValues[(int)EARFaceBlendShape::MouthSmileLeft]) *
                                              0.5f;
                SecondValue *= SmilingMultiply;
                UpdateValue *= (1 - SmilingMultiply);
                SwitcherComponent->SetFaceValue(SmilingMultiply);
            }
            else
            {
                const float FaceMultiply = SwitcherComponent->GetCurrentValue(EDMSwitchTarget::Face);
                SecondValue *= FaceMultiply;
                UpdateValue *= (1 - FaceMultiply);
            }

            Super::SetMorphTarget(InDefaultType, DefaultValue, CalibrationBlendShape->CustomName, SecondValue);
        }
    }

    Super::SetMorphTarget(InDefaultType, DefaultValue, UpdateName, UpdateValue);
}

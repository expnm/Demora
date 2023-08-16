#pragma once

#include "CoreMinimal.h"
#include "Types/DMAttachmentTypes.h"
#include "Types/DMStringLiterals.h"

#include "DMMaterialTypes.generated.h"

const int EyeMaterialSocketID = 1;
const int DressMaterialSocketID = 2;

UENUM(BlueprintType)
enum class EDMMaterialParameter : uint8
{
    Eye2,
    SphereRadius1,
    SpherePos1,
    SphereRadius2,
    SpherePos2,
    ShowTails,
    HideSimulated,
    MAX
};

const TMap<EDMMaterialParameter, FName> DMMaterialParameters = {
    {EDMMaterialParameter::Eye2, MaterialParameterName::Eye2},
    {EDMMaterialParameter::SphereRadius1, MaterialParameterName::SphereRadius1},
    {EDMMaterialParameter::SpherePos1, MaterialParameterName::SpherePos1},
    {EDMMaterialParameter::SphereRadius2, MaterialParameterName::SphereRadius2},
    {EDMMaterialParameter::SpherePos2, MaterialParameterName::SpherePos2},
    {EDMMaterialParameter::ShowTails, MaterialParameterName::ShowTails},
    {EDMMaterialParameter::HideSimulated, MaterialParameterName::HideSimulated},
};

USTRUCT(BlueprintType)
struct FDMMaterialAttachmentData
{
    GENERATED_USTRUCT_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EDMAttachment Type = EDMAttachment::Body;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString CustomName = TEXT("");

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int MaterialSocketID = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UMaterialInterface* SourceMaterial = nullptr;

    void SetMaterial(UMaterialInstanceDynamic* InMaterial) { Material = InMaterial; }
    UMaterialInstanceDynamic* GetMaterial() const { return Material; }
    bool IsMaterialAvailable() const { return Material ? true : false; }

private:
    UMaterialInstanceDynamic* Material = nullptr;
};

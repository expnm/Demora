#pragma once

#include "CoreMinimal.h"

#include "DMControlPanelTypes.generated.h"

UENUM(BlueprintType)
enum class EDMControlPanelEvent : uint8
{
    SwitchFace,
    SwitchHair,
    HideHat,
    HideSkirt,
    HideHands,
    Child,
    Animation1,
    Animation2,
    Animation3,
};

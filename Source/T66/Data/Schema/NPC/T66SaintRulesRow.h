#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "T66SaintRulesRow.generated.h"

USTRUCT(BlueprintType)
struct T66_API FT66SaintRulesRow : public FTableRowBase
{
	GENERATED_BODY()

	// Bible: Saint Beam Aura kills enemies; can kill player if caught. :contentReference[oaicite:9]{index=9}
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bEnableSaintBeamAura = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0.0"))
	float AuraRadius = 1400.0f;
};

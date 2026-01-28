#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "T66OuroborosRulesRow.generated.h"

USTRUCT(BlueprintType)
struct T66_API FT66OuroborosRulesRow : public FTableRowBase
{
	GENERATED_BODY()

	// Bible: Ouroboros is a danger landmark; after killed, threat disabled for rest of stage. :contentReference[oaicite:12]{index=12}
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsDangerZone = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0.0"))
	float DangerRadius = 1600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bThreatDisabledAfterDefeat = true;
};

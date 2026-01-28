#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "T66VendorRulesRow.generated.h"

USTRUCT(BlueprintType)
struct T66_API FT66VendorRulesRow : public FTableRowBase
{
	GENERATED_BODY()

	// How many sale offers are shown at once.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0"))
	int32 OfferCount = 6;

	// Bible: sells Normal Items only; never sells Cursed items. :contentReference[oaicite:5]{index=5}
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bSellsNormalItemsOnly = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bNeverSellsCursedItems = true;

	// Anger threshold roll target (tunable). Bible target: ~1200g. :contentReference[oaicite:6]{index=6}
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0"))
	int32 AngerThresholdTarget = 1200;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bAllowStealing = true;
};

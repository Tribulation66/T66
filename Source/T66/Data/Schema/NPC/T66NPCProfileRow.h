#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "T66NPCProfileRow.generated.h"

UENUM(BlueprintType)
enum class ET66NPCType : uint8
{
	Vendor     UMETA(DisplayName="Vendor"),
	Gambler    UMETA(DisplayName="Gambler"),
	Saint      UMETA(DisplayName="Saint"),
	Trickster  UMETA(DisplayName="Trickster"),
	Ouroboros  UMETA(DisplayName="Ouroboros"),
};

UENUM(BlueprintType)
enum class ET66NPCInteractStyle : uint8
{
	Shop        UMETA(DisplayName="Shop"),
	Casino      UMETA(DisplayName="Casino"),
	Dialogue    UMETA(DisplayName="Dialogue"),
	Gate        UMETA(DisplayName="Gate"),
	DangerZone  UMETA(DisplayName="DangerZone"),
};

USTRUCT(BlueprintType)
struct T66_API FT66NPCProfileRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ET66NPCType NPCType = ET66NPCType::Vendor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ET66NPCInteractStyle InteractStyle = ET66NPCInteractStyle::Dialogue;

	// Bible: "Not a pause" (stage time continues while UI open). :contentReference[oaicite:2]{index=2}
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bStageTimeContinuesDuringInteraction = true;

	// Bible: standard NPC safe bubble blocks enemies + blocks miasma while inside. :contentReference[oaicite:3]{index=3}
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bHasSafeBubble = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0.0"))
	float SafeBubbleRadius = 900.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bEnemiesBlockedByBubble = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bEnemiesCannotSpawnInsideBubble = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bMiasmaBlockedInsideBubble = true;

	// Bible: Players cannot attack/shoot while inside bubble (attack input ignored). :contentReference[oaicite:4]{index=4}
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bBlockAttackInputInsideBubble = true;

	// Optional: lightweight identifier to help link level/building logic later.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName BuildingOrLandmarkId = NAME_None;
};

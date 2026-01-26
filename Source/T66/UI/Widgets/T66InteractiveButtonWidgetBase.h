#pragma once

#include "CoreMinimal.h"
#include "UI/Widgets/T66WidgetBase.h"
#include "GameplayTagContainer.h"
#include "T66InteractiveButtonWidgetBase.generated.h"

/**
 * Base class for ALL interactive/clickable buttons in T66.
 * This guarantees the contract variables exist via inheritance
 * (no more patching variables inside the Blueprint manually).
 */
UCLASS(BlueprintType, Blueprintable)
class T66_API UT66InteractiveButtonWidgetBase : public UT66ComponentWidgetBase
{
	GENERATED_BODY()

public:
	UT66InteractiveButtonWidgetBase(const FObjectInitializer& ObjectInitializer);

	/** Focus/navigation identity for this button (used by focus system + controller nav). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "T66|UI|Button", meta = (Categories = "Focus"))
	FGameplayTag ControlID;

	/** What action this button represents (Accept/Back/Open/TabLeft/etc). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "T66|UI|Button", meta = (Categories = "UI.Action"))
	FGameplayTag ActionTag;

	/**
	 * Optional routing target.
	 * Example usage: UI.Screen.Settings, UI.Modal.ConfirmQuit, UI.Overlay.Map
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "T66|UI|Button", meta = (Categories = "UI"))
	FGameplayTag RouteTag;

	/** Text shown on the button (if the button style uses text). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "T66|UI|Button")
	FText LabelText;

	/** Whether this button begins enabled. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "T66|UI|Button")
	bool bIsEnabledByDefault;

	/** Whether this button style should show an icon region (if the widget supports icons). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "T66|UI|Button")
	bool bShowIcon;
};

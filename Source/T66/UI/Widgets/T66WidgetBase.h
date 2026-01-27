#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "T66WidgetBase.generated.h"

UENUM(BlueprintType)
enum class ET66UISurfaceType : uint8
{
	Unknown     UMETA(DisplayName = "Unknown"),
	Screen      UMETA(DisplayName = "Screen"),
	Modal       UMETA(DisplayName = "Modal"),
	Overlay     UMETA(DisplayName = "Overlay"),
	Component   UMETA(DisplayName = "Component"),
};

/**
 * Base widget for all T66 UI assets (screens, modals, overlays, components).
 *
 * This is where we store the authoritative IDs (GameplayTags) that the Editor
 * "Widget Contracts" tool will enforce/repair automatically.
 */
UCLASS(BlueprintType, Blueprintable)
class T66_API UT66WidgetBase : public UUserWidget
{
	GENERATED_BODY()

public:
	UT66WidgetBase(const FObjectInitializer& ObjectInitializer);

	/** What kind of UI surface this widget is. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "T66|UI")
	ET66UISurfaceType SurfaceType;

	/**
	 * Canonical identity for a UI surface (ex: UI.Screen.MainMenu, UI.Modal.Settings, UI.Overlay.Map).
	 *
	 * - Mandatory for Screen/Modal/Overlay surfaces.
	 * - Must remain None for Components.
	 *
	 * The contracts tool will set/repair this automatically.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "T66|UI", meta = (Categories = "UI"))
	FGameplayTag SurfaceID;

	/**
	 * Optional focus target the UI system can use when the widget is shown.
	 * Example tags live under Focus.* (Focus.Primary, Focus.List, Focus.Cancel, etc).
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "T66|UI", meta = (Categories = "Focus"))
	FGameplayTag DefaultFocusID;
};

/** Base class for any full-screen UI "Screen". */
UCLASS(BlueprintType, Blueprintable)
class T66_API UT66ScreenWidgetBase : public UT66WidgetBase
{
	GENERATED_BODY()

public:
	UT66ScreenWidgetBase(const FObjectInitializer& ObjectInitializer);
};

/** Base class for any blocking UI "Modal". */
UCLASS(BlueprintType, Blueprintable)
class T66_API UT66ModalWidgetBase : public UT66WidgetBase
{
	GENERATED_BODY()

public:
	UT66ModalWidgetBase(const FObjectInitializer& ObjectInitializer);
};

/** Base class for any non-blocking UI "Overlay". */
UCLASS(BlueprintType, Blueprintable)
class T66_API UT66OverlayWidgetBase : public UT66WidgetBase
{
	GENERATED_BODY()

public:
	UT66OverlayWidgetBase(const FObjectInitializer& ObjectInitializer);
};

/** Base class for reusable UI components (buttons, panels, etc). */
UCLASS(BlueprintType, Blueprintable)
class T66_API UT66ComponentWidgetBase : public UT66WidgetBase
{
	GENERATED_BODY()

public:
	UT66ComponentWidgetBase(const FObjectInitializer& ObjectInitializer);
};

// Copyright Epic Games, Inc. All Rights Reserved.

#include "T66PlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "Blueprint/UserWidget.h"
#include "T66.h"
#include "Widgets/Input/SVirtualJoystick.h"

void AT66PlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Only do UI + touch controls for local player controllers
	if (IsLocalPlayerController())
	{
		// 1) Spawn the UI Root (WBP_UIRoot) first so it's the base layer
		CreateUIRootIfNeeded();

		// 2) Optional: spawn touch controls above it (mobile / forced touch)
		if (ShouldUseTouchControls())
		{
			if (!MobileControlsWidgetClass)
			{
				UE_LOG(LogT66, Warning, TEXT("MobileControlsWidgetClass is not set. Touch controls will not spawn."));
				return;
			}

			MobileControlsWidget = CreateWidget<UUserWidget>(this, MobileControlsWidgetClass);
			if (MobileControlsWidget)
			{
				// Put touch controls above the UI root
				MobileControlsWidget->AddToPlayerScreen(100);
			}
			else
			{
				UE_LOG(LogT66, Error, TEXT("Could not spawn mobile controls widget."));
			}
		}
	}
}

void AT66PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Only add IMCs for local player controllers
	if (IsLocalPlayerController())
	{
		// Add Input Mapping Contexts
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				if (CurrentContext)
				{
					Subsystem->AddMappingContext(CurrentContext, 0);
				}
			}

			// Only add these IMCs if we're not using mobile touch input
			if (!ShouldUseTouchControls())
			{
				for (UInputMappingContext* CurrentContext : MobileExcludedMappingContexts)
				{
					if (CurrentContext)
					{
						Subsystem->AddMappingContext(CurrentContext, 0);
					}
				}
			}
		}
	}
}

bool AT66PlayerController::ShouldUseTouchControls() const
{
	// Are we on a mobile platform? Should we force touch?
	return SVirtualJoystick::ShouldDisplayTouchInterface() || bForceTouchControls;
}

void AT66PlayerController::CreateUIRootIfNeeded()
{
	if (UIRootWidget)
	{
		return; // already created
	}

	if (!UIRootWidgetClass)
	{
		UE_LOG(LogT66, Warning, TEXT("UIRootWidgetClass is not set. UI Root will not spawn."));
		return;
	}

	UIRootWidget = CreateWidget<UUserWidget>(this, UIRootWidgetClass);
	if (!UIRootWidget)
	{
		UE_LOG(LogT66, Error, TEXT("Failed to create UI Root widget instance."));
		return;
	}

	// ZOrder 0 = base UI layer
	UIRootWidget->AddToPlayerScreen(0);

	UE_LOG(LogT66, Display, TEXT("Spawned UI Root: %s"), *GetNameSafe(UIRootWidget));
}

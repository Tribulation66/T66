#include "T66/Items/White/T66WhiteModeSubsystem.h"

void UT66WhiteModeSubsystem::StartWhiteModeById(FName WhiteItemId)
{
	if (bWhiteModeActive)
	{
		UE_LOG(LogTemp, Display, TEXT("[T66WhiteModeSubsystem] White mode already active; ignoring start (%s)."), *WhiteItemId.ToString());
		return;
	}

	bWhiteModeActive = true;
	UE_LOG(LogTemp, Display, TEXT("[T66WhiteModeSubsystem] StartWhiteModeById: %s (TODO: duration + boss/mob rules via DT_WhiteItems)"),
		*WhiteItemId.ToString());
}

void UT66WhiteModeSubsystem::StopWhiteMode()
{
	if (!bWhiteModeActive)
	{
		return;
	}

	bWhiteModeActive = false;
	UE_LOG(LogTemp, Display, TEXT("[T66WhiteModeSubsystem] StopWhiteMode"));
}

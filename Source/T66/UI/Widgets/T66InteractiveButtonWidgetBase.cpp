#include "UI/Widgets/T66InteractiveButtonWidgetBase.h"

UT66InteractiveButtonWidgetBase::UT66InteractiveButtonWidgetBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsEnabledByDefault = true;
	bShowIcon = false;
	LabelText = FText::GetEmpty();
}

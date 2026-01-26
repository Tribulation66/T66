#include "UI/Widgets/T66WidgetBase.h"

UT66WidgetBase::UT66WidgetBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SurfaceType = ET66UISurfaceType::Unknown;
}

UT66ScreenWidgetBase::UT66ScreenWidgetBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SurfaceType = ET66UISurfaceType::Screen;
}

UT66ModalWidgetBase::UT66ModalWidgetBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SurfaceType = ET66UISurfaceType::Modal;
}

UT66OverlayWidgetBase::UT66OverlayWidgetBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SurfaceType = ET66UISurfaceType::Overlay;
}

UT66ComponentWidgetBase::UT66ComponentWidgetBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SurfaceType = ET66UISurfaceType::Component;
}

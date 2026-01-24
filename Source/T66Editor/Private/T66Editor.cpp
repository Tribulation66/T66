#include "Modules/ModuleManager.h"
#include "ToolMenus.h"
#include "Editor.h"

#include "T66RegistryToolsSubsystem.h"
#include "T66WidgetLayoutToolsSubsystem.h"

class FT66EditorModule : public IModuleInterface
{
public:
	virtual void StartupModule() override
	{
		UToolMenus::RegisterStartupCallback(
			FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FT66EditorModule::RegisterMenus));
	}

	virtual void ShutdownModule() override
	{
		UToolMenus::UnRegisterStartupCallback(this);
		UToolMenus::UnregisterOwner(this);
	}

private:
	void RegisterMenus()
	{
		FToolMenuOwnerScoped OwnerScoped(this);

		// ============================================================
		// ✅ MAIN MENU: Tools -> T66 Tools
		// ============================================================
		UToolMenu* ToolsMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Tools");
		if (!ToolsMenu)
		{
			return;
		}

		FToolMenuSection& Section = ToolsMenu->FindOrAddSection("T66Tools");

		// Helper to get our registry tools subsystem cleanly
		auto GetRegistryToolsSubsystem = []() -> UT66RegistryToolsSubsystem*
			{
				if (!GEditor)
				{
					return nullptr;
				}
				return GEditor->GetEditorSubsystem<UT66RegistryToolsSubsystem>();
			};

		// Helper to get our widget layout tools subsystem cleanly
		auto GetWidgetLayoutToolsSubsystem = []() -> UT66WidgetLayoutToolsSubsystem*
			{
				if (!GEditor)
				{
					return nullptr;
				}
				return GEditor->GetEditorSubsystem<UT66WidgetLayoutToolsSubsystem>();
			};

		// ✅ 1) Fill Surface Registry
		{
			FToolUIActionChoice Action(FExecuteAction::CreateLambda([GetRegistryToolsSubsystem]()
				{
					if (UT66RegistryToolsSubsystem* Tools = GetRegistryToolsSubsystem())
					{
						Tools->FillSurfaceRegistry();
					}
				}));

			Section.AddEntry(FToolMenuEntry::InitMenuEntry(
				FName("T66Tools_FillSurfaceRegistry"),
				FText::FromString("T66 Tools: Fill Surface Registry"),
				FText::FromString("Auto-fill DA_UIRegistry_Surfaces by scanning WBP_* widgets."),
				FSlateIcon(),
				Action
			));
		}

		// ✅ 2) Fill Input Context Registry
		{
			FToolUIActionChoice Action(FExecuteAction::CreateLambda([GetRegistryToolsSubsystem]()
				{
					if (UT66RegistryToolsSubsystem* Tools = GetRegistryToolsSubsystem())
					{
						Tools->FillInputContextRegistry();
					}
				}));

			Section.AddEntry(FToolMenuEntry::InitMenuEntry(
				FName("T66Tools_FillInputContextRegistry"),
				FText::FromString("T66 Tools: Fill Input Context Registry"),
				FText::FromString("Auto-fill DA_T66UIInputContexts by scanning IMC_UI_* assets."),
				FSlateIcon(),
				Action
			));
		}

		// ✅ 3) Apply Default UI Keybinds
		{
			FToolUIActionChoice Action(FExecuteAction::CreateLambda([GetRegistryToolsSubsystem]()
				{
					if (UT66RegistryToolsSubsystem* Tools = GetRegistryToolsSubsystem())
					{
						Tools->ApplyDefaultUIKeybinds();
					}
				}));

			Section.AddEntry(FToolMenuEntry::InitMenuEntry(
				FName("T66Tools_ApplyDefaultUIKeybinds"),
				FText::FromString("T66 Tools: Apply Default UI Keybinds"),
				FText::FromString("Applies default keyboard + gamepad binds to all IMC_UI_* assets."),
				FSlateIcon(),
				Action
			));
		}

		// ✅ 4) Create/Repair UI Theme Assets
		{
			FToolUIActionChoice Action(FExecuteAction::CreateLambda([GetRegistryToolsSubsystem]()
				{
					if (UT66RegistryToolsSubsystem* Tools = GetRegistryToolsSubsystem())
					{
						Tools->CreateOrRepairUIThemeAssets();
					}
				}));

			Section.AddEntry(FToolMenuEntry::InitMenuEntry(
				FName("T66Tools_CreateOrRepairUIThemeAssets"),
				FText::FromString("T66 Tools: Create/Repair UI Theme Assets"),
				FText::FromString("Ensures DA_UITheme_* exist in the canonical folder and fills safe starter tokens."),
				FSlateIcon(),
				Action
			));
		}

		// ✅ 5) Create/Repair UI Button Components (WBP_Comp_Button_Action)
		{
			FToolUIActionChoice Action(FExecuteAction::CreateLambda([GetWidgetLayoutToolsSubsystem]()
				{
					if (UT66WidgetLayoutToolsSubsystem* Tools = GetWidgetLayoutToolsSubsystem())
					{
						Tools->CreateOrRepairUIButtonComponents();
					}
				}));

			Section.AddEntry(FToolMenuEntry::InitMenuEntry(
				FName("T66Tools_CreateOrRepairUIButtonComponents"),
				FText::FromString("T66 Tools: Create/Repair UI Button Components"),
				FText::FromString("Repairs WBP_Comp_Button_Action (variables + minimal widget tree). Add/Repair only."),
				FSlateIcon(),
				Action
			));
		}

		// ✅ 6) Build Minimum Layouts for SELECTED Widget Blueprints (stub for now)
		{
			FToolUIActionChoice Action(FExecuteAction::CreateLambda([GetWidgetLayoutToolsSubsystem]()
				{
					if (UT66WidgetLayoutToolsSubsystem* Tools = GetWidgetLayoutToolsSubsystem())
					{
						Tools->BuildMinimumLayoutsForSelectedWidgetBlueprints();
					}
				}));

			Section.AddEntry(FToolMenuEntry::InitMenuEntry(
				FName("T66Tools_BuildMinimumLayouts_Selected"),
				FText::FromString("T66 Tools: Build Minimum Layouts (Selected Widgets)"),
				FText::FromString("Stamps minimum layouts for the Widget Blueprints you selected in the Content Browser. Add/Repair only."),
				FSlateIcon(),
				Action
			));
		}
	}
};

IMPLEMENT_MODULE(FT66EditorModule, T66Editor);

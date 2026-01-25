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

		// ✅ 6b) Build Minimum Layouts for ALL Screens (WBP_Screen_*)
		{
			FToolUIActionChoice Action(FExecuteAction::CreateLambda([GetWidgetLayoutToolsSubsystem]()
				{
					if (UT66WidgetLayoutToolsSubsystem* Tools = GetWidgetLayoutToolsSubsystem())
					{
						Tools->BuildMinimumLayoutsForAllScreenWidgetBlueprints();
					}
				}));

			Section.AddEntry(FToolMenuEntry::InitMenuEntry(
				FName("T66Tools_BuildMinimumLayouts_AllScreens"),
				FText::FromString("T66 Tools: Build Minimum Layouts (All Screens)"),
				FText::FromString("Stamps minimum layouts for ALL screens (WBP_Screen_*). Explicit recipes only. Add/Repair only."),
				FSlateIcon(),
				Action
			));
		}

		// ✅ 7) Build Minimum Layouts for ALL Surfaces (Screens + Modals + Overlays + Tooltips)
		{
			FToolUIActionChoice Action(FExecuteAction::CreateLambda([GetWidgetLayoutToolsSubsystem]()
				{
					if (UT66WidgetLayoutToolsSubsystem* Tools = GetWidgetLayoutToolsSubsystem())
					{
						Tools->BuildMinimumLayoutsForAllSurfaceWidgetBlueprints();
					}
				}));

			Section.AddEntry(FToolMenuEntry::InitMenuEntry(
				FName("T66Tools_BuildMinimumLayouts_AllSurfaces"),
				FText::FromString("T66 Tools: Build Minimum Layouts (All Surfaces)"),
				FText::FromString("Stamps minimum layouts for ALL UI surfaces (Screens, Modals, Overlays, Tooltips). Explicit recipes only. Add/Repair only."),
				FSlateIcon(),
				Action
			));
		}

		// ✅ 8) Build Minimum Layouts for ALL Overlays (WBP_Ov_*)
		{
			FToolUIActionChoice Action(FExecuteAction::CreateLambda([GetWidgetLayoutToolsSubsystem]()
				{
					if (UT66WidgetLayoutToolsSubsystem* Tools = GetWidgetLayoutToolsSubsystem())
					{
						Tools->BuildMinimumLayoutsForAllOverlayWidgetBlueprints();
					}
				}));

			Section.AddEntry(FToolMenuEntry::InitMenuEntry(
				FName("T66Tools_BuildMinimumLayouts_AllOverlays"),
				FText::FromString("T66 Tools: Build Minimum Layouts (All Overlays)"),
				FText::FromString("Stamps minimum layouts for ALL overlays (WBP_Ov_*). Explicit recipes only. Add/Repair only."),
				FSlateIcon(),
				Action
			));
		}

		// ✅ 9) Build Minimum Layouts for ALL Modals (WBP_Modal_*)
		{
			FToolUIActionChoice Action(FExecuteAction::CreateLambda([GetWidgetLayoutToolsSubsystem]()
				{
					if (UT66WidgetLayoutToolsSubsystem* Tools = GetWidgetLayoutToolsSubsystem())
					{
						Tools->BuildMinimumLayoutsForAllModalWidgetBlueprints();
					}
				}));

			Section.AddEntry(FToolMenuEntry::InitMenuEntry(
				FName("T66Tools_BuildMinimumLayouts_AllModals"),
				FText::FromString("T66 Tools: Build Minimum Layouts (All Modals)"),
				FText::FromString("Stamps minimum layouts for ALL modals (WBP_Modal_*). Explicit recipes only. Add/Repair only."),
				FSlateIcon(),
				Action
			));
		}

		// ✅ 10) Build Minimum Layouts for ALL Tooltips (WBP_Tooltip_*)
		{
			FToolUIActionChoice Action(FExecuteAction::CreateLambda([GetWidgetLayoutToolsSubsystem]()
				{
					if (UT66WidgetLayoutToolsSubsystem* Tools = GetWidgetLayoutToolsSubsystem())
					{
						Tools->BuildMinimumLayoutsForAllTooltipWidgetBlueprints();
					}
				}));

			Section.AddEntry(FToolMenuEntry::InitMenuEntry(
				FName("T66Tools_BuildMinimumLayouts_AllTooltips"),
				FText::FromString("T66 Tools: Build Minimum Layouts (All Tooltips)"),
				FText::FromString("Stamps minimum layouts for ALL tooltips (WBP_Tooltip_*). Explicit recipes only. Add/Repair only."),
				FSlateIcon(),
				Action
			));
		}

		// ✅ 11) Build Minimum Layouts for ALL Components (Buttons + Text + UI_Blocks + Utility_UI)
		{
			FToolUIActionChoice Action(FExecuteAction::CreateLambda([GetWidgetLayoutToolsSubsystem]()
				{
					if (UT66WidgetLayoutToolsSubsystem* Tools = GetWidgetLayoutToolsSubsystem())
					{
						Tools->BuildMinimumLayoutsForAllComponentWidgetBlueprints();
					}
				}));

			Section.AddEntry(FToolMenuEntry::InitMenuEntry(
				FName("T66Tools_BuildMinimumLayouts_AllComponents"),
				FText::FromString("T66 Tools: Build Minimum Layouts (All Components)"),
				FText::FromString("Stamps minimum layouts for ALL UI components (Buttons, Text, UI_Blocks, Utility_UI). Explicit recipes only. Add/Repair only."),
				FSlateIcon(),
				Action
			));
		}

		// ✅ 12) Build Minimum Layouts for Component Folders (explicit recipes only)
		{
			FToolUIActionChoice Action(FExecuteAction::CreateLambda([GetWidgetLayoutToolsSubsystem]()
				{
					if (UT66WidgetLayoutToolsSubsystem* Tools = GetWidgetLayoutToolsSubsystem())
					{
						Tools->BuildMinimumLayoutsForAllButtonComponentWidgetBlueprints();
					}
				}));

			Section.AddEntry(FToolMenuEntry::InitMenuEntry(
				FName("T66Tools_BuildMinimumLayouts_AllButtonComponents"),
				FText::FromString("T66 Tools: Build Minimum Layouts (All Button Components)"),
				FText::FromString("Stamps minimum layouts for ALL button components under UI/Components/Button. Explicit recipes only. Add/Repair only."),
				FSlateIcon(),
				Action
			));
		}

		{
			FToolUIActionChoice Action(FExecuteAction::CreateLambda([GetWidgetLayoutToolsSubsystem]()
				{
					if (UT66WidgetLayoutToolsSubsystem* Tools = GetWidgetLayoutToolsSubsystem())
					{
						Tools->BuildMinimumLayoutsForAllTextComponentWidgetBlueprints();
					}
				}));

			Section.AddEntry(FToolMenuEntry::InitMenuEntry(
				FName("T66Tools_BuildMinimumLayouts_AllTextComponents"),
				FText::FromString("T66 Tools: Build Minimum Layouts (All Text Components)"),
				FText::FromString("Stamps minimum layouts for ALL text components under UI/Components/Text. Explicit recipes only. Add/Repair only."),
				FSlateIcon(),
				Action
			));
		}

		{
			FToolUIActionChoice Action(FExecuteAction::CreateLambda([GetWidgetLayoutToolsSubsystem]()
				{
					if (UT66WidgetLayoutToolsSubsystem* Tools = GetWidgetLayoutToolsSubsystem())
					{
						Tools->BuildMinimumLayoutsForAllUIBlocksComponentWidgetBlueprints();
					}
				}));

			Section.AddEntry(FToolMenuEntry::InitMenuEntry(
				FName("T66Tools_BuildMinimumLayouts_AllUIBlocksComponents"),
				FText::FromString("T66 Tools: Build Minimum Layouts (All UI_Blocks Components)"),
				FText::FromString("Stamps minimum layouts for ALL UI block components under UI/Components/UI_Blocks. Explicit recipes only. Add/Repair only."),
				FSlateIcon(),
				Action
			));
		}

		{
			FToolUIActionChoice Action(FExecuteAction::CreateLambda([GetWidgetLayoutToolsSubsystem]()
				{
					if (UT66WidgetLayoutToolsSubsystem* Tools = GetWidgetLayoutToolsSubsystem())
					{
						Tools->BuildMinimumLayoutsForAllUtilityUIComponentWidgetBlueprints();
					}
				}));

			Section.AddEntry(FToolMenuEntry::InitMenuEntry(
				FName("T66Tools_BuildMinimumLayouts_AllUtilityUIComponents"),
				FText::FromString("T66 Tools: Build Minimum Layouts (All Utility_UI Components)"),
				FText::FromString("Stamps minimum layouts for ALL utility UI components under UI/Components/Utility_UI. Explicit recipes only. Add/Repair only."),
				FSlateIcon(),
				Action
			));
		}
	}
};

IMPLEMENT_MODULE(FT66EditorModule, T66Editor);
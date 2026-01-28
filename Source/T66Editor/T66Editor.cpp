#include "Modules/ModuleManager.h"
#include "ToolMenus.h"
#include "Editor.h"

#include "T66RegistryTools.h"
#include "T66WidgetTools.h"

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

		UToolMenu* ToolsMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Tools");
		if (!ToolsMenu)
		{
			return;
		}

		FToolMenuSection& Section = ToolsMenu->FindOrAddSection("T66Tools");

		auto GetRegistryToolsSubsystem = []() -> UT66RegistryTools*
			{
				if (!GEditor) return nullptr;
				return GEditor->GetEditorSubsystem<UT66RegistryTools>();
			};

		auto GetWidgetTools = []() -> UT66WidgetTools*
			{
				if (!GEditor) return nullptr;
				return GEditor->GetEditorSubsystem<UT66WidgetTools>();
			};

		// 0) Core Data (DT-first): Create/Repair core DTs + DataCatalog
		{
			FToolUIActionChoice Action(FExecuteAction::CreateLambda([GetRegistryToolsSubsystem]()
				{
					if (UT66RegistryTools* Tools = GetRegistryToolsSubsystem())
					{
						Tools->CreateOrRepairCoreDataTablesAndCatalog();
					}
				}));

			Section.AddEntry(FToolMenuEntry::InitMenuEntry(
				FName("T66Tools_CreateOrRepairCoreDataTablesAndCatalog"),
				FText::FromString("T66 Tools: Create/Repair Core DataTables + DataCatalog"),
				FText::FromString("Creates DT_Items/DT_Enemies/DT_StageEffects/DT_Companions and wires DA_DataCatalog_Core. No manual Content Browser setup."),
				FSlateIcon(),
				Action
			));
		}

		// 1) Fill Surface Registry
		{
			FToolUIActionChoice Action(FExecuteAction::CreateLambda([GetRegistryToolsSubsystem]()
				{
					if (UT66RegistryTools* Tools = GetRegistryToolsSubsystem())
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

		// 2) Fill Input Context Registry
		{
			FToolUIActionChoice Action(FExecuteAction::CreateLambda([GetRegistryToolsSubsystem]()
				{
					if (UT66RegistryTools* Tools = GetRegistryToolsSubsystem())
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

		// 3) Apply Default UI Keybinds
		{
			FToolUIActionChoice Action(FExecuteAction::CreateLambda([GetRegistryToolsSubsystem]()
				{
					if (UT66RegistryTools* Tools = GetRegistryToolsSubsystem())
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

		// 4) Create/Repair UI Theme Assets
		{
			FToolUIActionChoice Action(FExecuteAction::CreateLambda([GetRegistryToolsSubsystem]()
				{
					if (UT66RegistryTools* Tools = GetRegistryToolsSubsystem())
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

		// 5) Contracts SAFE
		{
			FToolUIActionChoice Action(FExecuteAction::CreateLambda([GetWidgetTools]()
				{
					if (UT66WidgetTools* Tools = GetWidgetTools())
					{
						Tools->CreateOrRepairUIWidgetContracts_SelectedWidgets_Safe();
					}
				}));

			Section.AddEntry(FToolMenuEntry::InitMenuEntry(
				FName("T66Tools_UIWidgetContracts_Selected_Safe"),
				FText::FromString("T66 Tools: Create/Repair UI Widget Contracts (Selected Widgets)"),
				FText::FromString("Add/Repair only. Ensures required variables/IDs/components/children exist. No overwriting."),
				FSlateIcon(),
				Action
			));
		}

		// 6) Contracts FORCE
		{
			FToolUIActionChoice Action(FExecuteAction::CreateLambda([GetWidgetTools]()
				{
					if (UT66WidgetTools* Tools = GetWidgetTools())
					{
						Tools->CreateOrRepairUIWidgetContracts_SelectedWidgets_ForceOverwrite();
					}
				}));

			Section.AddEntry(FToolMenuEntry::InitMenuEntry(
				FName("T66Tools_UIWidgetContracts_Selected_Force"),
				FText::FromString("T66 Tools: Create/Repair UI Widget Contracts (FORCE Overwrite)"),
				FText::FromString("Deletes + rebuilds from recipe. Applies newest contracts/layout exactly."),
				FSlateIcon(),
				Action
			));
		}
	}
};

IMPLEMENT_MODULE(FT66EditorModule, T66Editor);

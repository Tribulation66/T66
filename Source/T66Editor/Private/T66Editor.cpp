#include "Modules/ModuleManager.h"
#include "ToolMenus.h"

#include "AssetRegistry/AssetData.h"
#include "ContentBrowserMenuContexts.h"
#include "Editor.h"

#include "T66RegistryToolsSubsystem.h"

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

		// ✅ Right-click menu for assets in the Content Browser
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("ContentBrowser.AssetContextMenu");
		if (!Menu)
		{
			return;
		}

		FToolMenuSection& Section = Menu->FindOrAddSection("T66Tools");

		// ============================================================
		// ✅ 1) SURFACE REGISTRY MENU ENTRY (DA_UIRegistry_Surfaces)
		// ============================================================
		Section.AddDynamicEntry(
			"T66_FillSurfaceRegistry",
			FNewToolMenuSectionDelegate::CreateLambda([](FToolMenuSection& InSection)
				{
					UContentBrowserAssetContextMenuContext* Context =
						InSection.FindContext<UContentBrowserAssetContextMenuContext>();

					if (!Context)
					{
						return;
					}

					bool bHasSurfaceRegistrySelected = false;
					for (const FAssetData& Asset : Context->SelectedAssets)
					{
						if (Asset.AssetName == FName(TEXT("DA_UIRegistry_Surfaces")))
						{
							bHasSurfaceRegistrySelected = true;
							break;
						}
					}

					if (!bHasSurfaceRegistrySelected)
					{
						return;
					}

					FToolUIActionChoice Action(FExecuteAction::CreateLambda([]()
						{
							if (!GEditor)
							{
								return;
							}

							if (UT66RegistryToolsSubsystem* Tools = GEditor->GetEditorSubsystem<UT66RegistryToolsSubsystem>())
							{
								Tools->FillSurfaceRegistry();
							}
						}));

					InSection.AddEntry(FToolMenuEntry::InitMenuEntry(
						FName("T66_FillSurfaceRegistry_Action"),
						FText::FromString("T66: Fill Surface Registry"),
						FText::FromString("Auto-fill DA_UIRegistry_Surfaces by scanning WBP_* widgets."),
						FSlateIcon(),
						Action
					));
				})
		);

		// ============================================================
		// ✅ 2) INPUT CONTEXT REGISTRY MENU ENTRY (DA_T66UIInputContexts)
		// ============================================================
		Section.AddDynamicEntry(
			"T66_FillInputContextRegistry",
			FNewToolMenuSectionDelegate::CreateLambda([](FToolMenuSection& InSection)
				{
					UContentBrowserAssetContextMenuContext* Context =
						InSection.FindContext<UContentBrowserAssetContextMenuContext>();

					if (!Context)
					{
						return;
					}

					bool bHasInputRegistrySelected = false;
					for (const FAssetData& Asset : Context->SelectedAssets)
					{
						if (Asset.AssetName == FName(TEXT("DA_T66UIInputContexts")))
						{
							bHasInputRegistrySelected = true;
							break;
						}
					}

					if (!bHasInputRegistrySelected)
					{
						return;
					}

					FToolUIActionChoice Action(FExecuteAction::CreateLambda([]()
						{
							if (!GEditor)
							{
								return;
							}

							if (UT66RegistryToolsSubsystem* Tools = GEditor->GetEditorSubsystem<UT66RegistryToolsSubsystem>())
							{
								Tools->FillInputContextRegistry();
							}
						}));

					InSection.AddEntry(FToolMenuEntry::InitMenuEntry(
						FName("T66_FillInputContextRegistry_Action"),
						FText::FromString("T66: Fill Input Context Registry"),
						FText::FromString("Auto-fill DA_T66UIInputContexts by scanning IMC_UI_* input contexts."),
						FSlateIcon(),
						Action
					));
				})
		);

		// ============================================================
		// ✅ 3) APPLY DEFAULT UI KEYBINDS (IMC_UI_* gets auto-mapped)
		//    Triggered by right-clicking DA_T66UIInputContexts
		// ============================================================
		Section.AddDynamicEntry(
			"T66_ApplyDefaultUIKeybinds",
			FNewToolMenuSectionDelegate::CreateLambda([](FToolMenuSection& InSection)
				{
					UContentBrowserAssetContextMenuContext* Context =
						InSection.FindContext<UContentBrowserAssetContextMenuContext>();

					if (!Context)
					{
						return;
					}

					bool bHasInputRegistrySelected = false;
					for (const FAssetData& Asset : Context->SelectedAssets)
					{
						if (Asset.AssetName == FName(TEXT("DA_T66UIInputContexts")))
						{
							bHasInputRegistrySelected = true;
							break;
						}
					}

					if (!bHasInputRegistrySelected)
					{
						return;
					}

					FToolUIActionChoice Action(FExecuteAction::CreateLambda([]()
						{
							if (!GEditor)
							{
								return;
							}

							if (UT66RegistryToolsSubsystem* Tools = GEditor->GetEditorSubsystem<UT66RegistryToolsSubsystem>())
							{
								// ⚠️ This will clear existing mappings for those actions in IMC_UI_* and apply defaults
								Tools->ApplyDefaultUIKeybinds();
							}
						}));

					InSection.AddEntry(FToolMenuEntry::InitMenuEntry(
						FName("T66_ApplyDefaultUIKeybinds_Action"),
						FText::FromString("T66: Apply Default UI Keybinds"),
						FText::FromString("Applies default keyboard + gamepad binds to all IMC_UI_* assets."),
						FSlateIcon(),
						Action
					));
				})
		);
	}
};

IMPLEMENT_MODULE(FT66EditorModule, T66Editor);

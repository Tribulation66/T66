#include "T66MainMenuWireCommandlet.h"

#include "WidgetBlueprint.h"
#include "Blueprint/WidgetTree.h"
#include "EditorAssetLibrary.h"
#include "GameplayTagContainer.h"
#include "UObject/UnrealType.h"

static const TCHAR* T66_MAINMENU_WBP_PATH = TEXT("/Game/Tribulation66/Content/UI/Screens/WBP_Screen_MainMenu.WBP_Screen_MainMenu");

namespace
{
	static bool SetGameplayTagProperty(UObject* Obj, const FName PropertyName, const FGameplayTag& Tag, const bool bForce)
	{
		if (!Obj || !Tag.IsValid())
		{
			return false;
		}

		FProperty* Prop = Obj->GetClass()->FindPropertyByName(PropertyName);
		FStructProperty* StructProp = CastField<FStructProperty>(Prop);
		if (!StructProp || StructProp->Struct != TBaseStructure<FGameplayTag>::Get())
		{
			return false;
		}

		FGameplayTag* TagPtr = StructProp->ContainerPtrToValuePtr<FGameplayTag>(Obj);
		if (!TagPtr)
		{
			return false;
		}

		if (!bForce && TagPtr->IsValid())
		{
			return false;
		}

		*TagPtr = Tag;
		return true;
	}

	static bool SetTextProperty(UObject* Obj, const FName PropertyName, const FText& Text, const bool bForce)
	{
		if (!Obj)
		{
			return false;
		}

		FProperty* Prop = Obj->GetClass()->FindPropertyByName(PropertyName);
		FTextProperty* TextProp = CastField<FTextProperty>(Prop);
		if (!TextProp)
		{
			return false;
		}

		FText* TextPtr = TextProp->ContainerPtrToValuePtr<FText>(Obj);
		if (!TextPtr)
		{
			return false;
		}

		if (!bForce && !TextPtr->IsEmpty())
		{
			return false;
		}

		*TextPtr = Text;
		return true;
	}

	static bool WireButton(
		UWidgetBlueprint* BP,
		const FName WidgetName,
		const FGameplayTag& ControlID,
		const FGameplayTag& ActionTag,
		const FGameplayTag& RouteTag,
		const FText& Label,
		const bool bForce,
		const bool bSetLabel)
	{
		if (!BP || !BP->WidgetTree)
		{
			return false;
		}

		UWidget* Widget = BP->WidgetTree->FindWidget(WidgetName);
		if (!Widget)
		{
			UE_LOG(LogTemp, Warning, TEXT("[T66MainMenuWire] Missing widget: %s"), *WidgetName.ToString());
			return false;
		}

		bool bAny = false;
		bAny |= SetGameplayTagProperty(Widget, FName(TEXT("ControlID")), ControlID, bForce);
		bAny |= SetGameplayTagProperty(Widget, FName(TEXT("ActionTag")), ActionTag, bForce);
		bAny |= SetGameplayTagProperty(Widget, FName(TEXT("RouteTag")), RouteTag, bForce);

		if (bSetLabel)
		{
			bAny |= SetTextProperty(Widget, FName(TEXT("LabelText")), Label, bForce);
		}

		return bAny;
	}
}

UT66MainMenuWireCommandlet::UT66MainMenuWireCommandlet()
{
	IsClient = false;
	IsEditor = true;
	IsServer = false;
	LogToConsole = true;
}

int32 UT66MainMenuWireCommandlet::Main(const FString& Params)
{
	const bool bForce = Params.Contains(TEXT("Force"), ESearchCase::IgnoreCase);

	UObject* Loaded = UEditorAssetLibrary::LoadAsset(T66_MAINMENU_WBP_PATH);
	UWidgetBlueprint* BP = Cast<UWidgetBlueprint>(Loaded);
	if (!BP)
	{
		UE_LOG(LogTemp, Error, TEXT("[T66MainMenuWire] Failed to load WBP_Screen_MainMenu at: %s"), T66_MAINMENU_WBP_PATH);
		return 1;
	}

	BP->Modify();

	const FGameplayTag FocusPrimary = FGameplayTag::RequestGameplayTag(FName(TEXT("Focus.Nav.Primary")), /*ErrorIfNotFound=*/false);
	const FGameplayTag RoutePartySize = FGameplayTag::RequestGameplayTag(FName(TEXT("UI.Screen.PartySizePicker")), false);
	const FGameplayTag RouteSaveSlots = FGameplayTag::RequestGameplayTag(FName(TEXT("UI.Screen.SaveSlots")), false);
	const FGameplayTag RouteAchievements = FGameplayTag::RequestGameplayTag(FName(TEXT("UI.Screen.Achievements")), false);
	const FGameplayTag RouteAccountStatus = FGameplayTag::RequestGameplayTag(FName(TEXT("UI.Screen.AccountStatus")), false);
	const FGameplayTag RouteSettings = FGameplayTag::RequestGameplayTag(FName(TEXT("UI.Modal.Settings")), false);
	const FGameplayTag RouteLanguage = FGameplayTag::RequestGameplayTag(FName(TEXT("UI.Modal.Language")), false);
	const FGameplayTag RouteQuitConfirm = FGameplayTag::RequestGameplayTag(FName(TEXT("UI.Modal.QuitConfirm")), false);

	const FGameplayTag ActionStartNew = FGameplayTag::RequestGameplayTag(FName(TEXT("UI.Action.StartNewGame")), false);
	const FGameplayTag ActionLoadGame = FGameplayTag::RequestGameplayTag(FName(TEXT("UI.Action.LoadGame")), false);
	const FGameplayTag ActionQuit = FGameplayTag::RequestGameplayTag(FName(TEXT("UI.Action.QuitGame")), false);

	int32 WiredCount = 0;
	WiredCount += WireButton(BP, FName(TEXT("Button_NewGame")), FocusPrimary, ActionStartNew, RoutePartySize, FText::FromString(TEXT("New Game")), bForce, /*bSetLabel=*/true);
	WiredCount += WireButton(BP, FName(TEXT("Button_LoadGame")), FGameplayTag(), ActionLoadGame, RouteSaveSlots, FText::FromString(TEXT("Load Game")), bForce, true);
	WiredCount += WireButton(BP, FName(TEXT("Button_Settings")), FGameplayTag(), FGameplayTag(), RouteSettings, FText::FromString(TEXT("Settings")), bForce, true);
	WiredCount += WireButton(BP, FName(TEXT("Button_Achievements")), FGameplayTag(), FGameplayTag(), RouteAchievements, FText::FromString(TEXT("Achievements")), bForce, true);
	WiredCount += WireButton(BP, FName(TEXT("Button_AccountStatus")), FGameplayTag(), FGameplayTag(), RouteAccountStatus, FText::FromString(TEXT("Account Status")), bForce, true);
	WiredCount += WireButton(BP, FName(TEXT("Button_LanguageIcon")), FGameplayTag(), FGameplayTag(), RouteLanguage, FText(), bForce, /*bSetLabel=*/false);
	WiredCount += WireButton(BP, FName(TEXT("Button_QuitIcon")), FGameplayTag(), ActionQuit, RouteQuitConfirm, FText(), bForce, false);

	BP->MarkPackageDirty();
	const bool bSaved = UEditorAssetLibrary::SaveLoadedAsset(BP, /*bOnlyIfIsDirty=*/true);

	UE_LOG(LogTemp, Display, TEXT("[T66MainMenuWire] Wired=%d | Saved=%s"), WiredCount, bSaved ? TEXT("true") : TEXT("false"));
	return bSaved ? 0 : 1;
}

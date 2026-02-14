// Copyright Technical Artist - Jiahao.Chan, Individual. All Rights Reserved.

#include "SecondaryLevelToolMenu.h"

#include "ContentBrowserMenuContexts.h"
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"

#include "SecondaryLevel/SecondaryLevelViewport.h"

#define LOCTEXT_NAMESPACE "UltraUnrealToolkitToolBar"

void FSecondaryLevelToolMenu::Startup(void* Owner)
{
	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateStatic(&FSecondaryLevelToolMenu::RegisterMenus));
}

void FSecondaryLevelToolMenu::Shutdown(void* Owner)
{
	UToolMenus::UnRegisterStartupCallback(Owner);
	UToolMenus::UnregisterOwner(Owner);
}

void FSecondaryLevelToolMenu::RegisterMenus()
{
	// Expand asset context menu.
	if (UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("ContentBrowser.AssetContextMenu.World"))
	{
		FToolMenuSection& DynamicSection = Menu->AddDynamicSection("SecondaryLevelDynamicSection", FNewToolMenuDelegate::CreateLambda([](UToolMenu* ToolMenu)
		{
			if (UContentBrowserAssetContextMenuContext* AssetMenuContext = ToolMenu->Context.FindContext<UContentBrowserAssetContextMenuContext>())
			{
				if (AssetMenuContext->CommonClass == UWorld::StaticClass())
				{
					FToolMenuSection& Section = ToolMenu->FindOrAddSection("SecondaryLevelActions", LOCTEXT("SecondaryLevelSection", "参考预览场景"));
					Section.AddMenuEntry(
						FName("EditAsSecondaryLevel"),
						LOCTEXT("EditAsSecondaryLevel_Label", "作为 参考预览场景 打开……"),
						LOCTEXT("EditAsSecondaryLevel_Tip", "将此场景作为参考预览场景打开。"),
						FSlateIcon(FAppStyle::GetAppStyleSetName(), TEXT("ContentBrowser.AssetActions.OpenReadOnly")),
						FUIAction(FExecuteAction::CreateStatic(&FSecondaryLevelToolMenu::OnLevelSelected, AssetMenuContext->SelectedAssets)));
				}
			}
		}), FToolMenuInsert(NAME_None, EToolMenuInsertType::First));
	}
}

void FSecondaryLevelToolMenu::OnPickLevelClicked()
{
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");

	TSharedRef<SWindow> PickerWindow = SNew(SWindow)
		.Title(LOCTEXT("SecondaryLevelPickerWindowTitle_Label", "请选择参考预览场景"))
		.ClientSize(FVector2D(900, 600))
		.AutoCenter(EAutoCenter::PrimaryWorkArea)
		.SupportsMaximize(true)
		.SupportsMinimize(false);
	TWeakPtr<SWindow> WeakWindowPtr = PickerWindow;

	FAssetPickerConfig PickerConfig;
	PickerConfig.Filter.ClassPaths.Add(UWorld::StaticClass()->GetClassPathName());
	PickerConfig.Filter.bRecursiveClasses = true;
	PickerConfig.SelectionMode = ESelectionMode::Type::Multi;
	PickerConfig.InitialAssetViewType = EAssetViewType::Type::List;
	PickerConfig.OnAssetEnterPressed = FOnAssetEnterPressed::CreateLambda(
		[WeakWindowPtr](const TArray<FAssetData>& SelectedAssets)
		{
			OnLevelSelected(SelectedAssets);
			if (WeakWindowPtr.IsValid())
			{
				WeakWindowPtr.Pin()->RequestDestroyWindow();
			}
		});
	PickerConfig.OnAssetDoubleClicked = FOnAssetDoubleClicked::CreateLambda(
		[WeakWindowPtr](const FAssetData& AssetData)
		{
			if (AssetData.IsValid())
			{
				OnLevelSelected({AssetData});
			}
			if (WeakWindowPtr.IsValid())
			{
				WeakWindowPtr.Pin()->RequestDestroyWindow();
			}
		});
	TSharedRef<SWidget> PickerWidget = ContentBrowserModule.Get().CreateAssetPicker(PickerConfig);

	PickerWindow->SetContent(PickerWidget);
	FSlateApplication::Get().AddWindow(PickerWindow);
}

void FSecondaryLevelToolMenu::OnLevelSelected(TArray<FAssetData> LevelAssets)
{
	if (LevelAssets.Num() == 0)
	{
		return;
	}
	FScopedSlowTask SlowTask(LevelAssets.Num(), LOCTEXT("OpeningSecondaryLevels", "正在加载并打开场景……"));
	SlowTask.MakeDialog();
	const TSharedRef<FGlobalTabmanager>& GlobalTabManager = FGlobalTabmanager::Get();
	for (const FAssetData& Level : LevelAssets)
	{
		const FName TabId = FName(FString(TEXT("SecondaryLevel_")) + Level.PackageName.ToString());
        GlobalTabManager->RegisterNomadTabSpawner(
        	TabId,
        	FOnSpawnTab::CreateLambda([Level](const FSpawnTabArgs& Args)
        		{
        			return SNew(SDockTab)
        			.TabRole(NomadTab)
        			.TabColorScale(FLinearColor(1.0, 0.05, 0.05))
        			.IconColor(FLinearColor::Black)
        			[
        				SNew(SSecondaryLevelViewport)
        				.LevelPath(Level.GetSoftObjectPath())
        			];
        		}))
        	.SetDisplayName(FText::Format(LOCTEXT("TabDisplayName", "{0}(副本)"), FText::FromName(Level.AssetName)))
        	.SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tabs.Levels"))
        	.SetMenuType(ETabSpawnerMenuType::Hidden);
        GlobalTabManager->TryInvokeTab(TabId);
		SlowTask.EnterProgressFrame(1);
	}
}
#undef LOCTEXT_NAMESPACE

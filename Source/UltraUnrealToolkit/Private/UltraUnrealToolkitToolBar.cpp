// Copyright Technical Artist - Jiahao.Chan, Individual. All Rights Reserved.

#include "UltraUnrealToolkitToolBar.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "EditorUtilitySubsystem.h"
#include "EditorUtilityWidgetBlueprint.h"
#include "IMaterialEditor.h"
#include "MaterialEditorContext.h"
#include "Materials/MaterialInstanceConstant.h"

#include "SecondaryLevel/SecondaryLevelToolMenu.h"
#include "UltraToolkitEditorUtility.h"

#define LOCTEXT_NAMESPACE "UltraUnrealToolkitToolBar"

void FUltraUnrealToolkitToolBar::Startup(void* Owner)
{
	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateStatic(&FUltraUnrealToolkitToolBar::RegisterMenus));
}

void FUltraUnrealToolkitToolBar::Shutdown(void* Owner)
{
	UToolMenus::UnRegisterStartupCallback(Owner);
	UToolMenus::UnregisterOwner(Owner);
}

void FUltraUnrealToolkitToolBar::RegisterMenus()
{
	if (UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.User"))
	{
		FToolMenuSection& Section = Menu->FindOrAddSection("UltraUnrealToolkit");
        Section.AddEntry(FToolMenuEntry::InitComboButton(
        	NAME_None,
        	FUIAction(FExecuteAction::CreateStatic(&FUltraUnrealToolkitToolBar::OpenEntryWidget)),
        	FNewToolMenuWidget::CreateLambda([](const FToolMenuContext& InToolMenuContext)
        	{
        		FMenuBuilder MenuBuilder(true, TSharedPtr<FUICommandList>());

        		{
        			MenuBuilder.BeginSection(NAME_None, LOCTEXT("Section_LevelEdit", "地编"));
                    MenuBuilder.AddMenuEntry(
                        LOCTEXT("SecondaryLevel_Label", "参考预览场景"),
                        LOCTEXT("SecondaryLevel_Tip", "参考预览场景"),
                        FSlateIcon(),
                        FUIAction(FExecuteAction::CreateStatic(&FSecondaryLevelToolMenu::OnPickLevelClicked)),
                        NAME_None,
                        EUserInterfaceActionType::Button);
                    MenuBuilder.EndSection();
        		}

        		return MenuBuilder.MakeWidget();
        	}),
        	FText(),
        	LOCTEXT("UltraUnrealToolkitButton_Tip", "Ultra Unreal Toolkit"),
        	FSlateIcon(TEXT("UltraUnrealToolkitStyle"), "UltraUnrealToolkit.Icon_64")
        ));
	}

	if (UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("AssetEditor.MaterialInstanceEditor.ToolBar"))
	{
		FToolMenuSection& Section = Menu->FindOrAddSection("UltraUnrealToolkit");

        Section.AddEntry(FToolMenuEntry::InitComboButton(
        	NAME_None,
        	FUIAction(),
        	FNewToolMenuWidget::CreateLambda([](const FToolMenuContext& InToolMenuContext)
        	{
        		FMenuBuilder MenuBuilder(true, TSharedPtr<FUICommandList>());

        		{
					MenuBuilder.BeginSection(NAME_None, LOCTEXT("Section_MaterialInstance", "材质实例"));
	                MenuBuilder.AddMenuEntry(
	                    LOCTEXT("CleanupMaterialInstanceInvisibleParameters_Label", "清除材质实例隐藏参数"),
	                    LOCTEXT("CleanupMaterialInstanceInvisibleParameters_Tip", "清除材质实例隐藏参数"),
	                    FSlateIcon(),
	                    FUIAction(FExecuteAction::CreateLambda([&InToolMenuContext]()
	                    {
                    		if (const UMaterialEditorMenuContext* SubMenuContext = InToolMenuContext.FindContext<UMaterialEditorMenuContext>(); SubMenuContext && SubMenuContext->MaterialEditor.IsValid())
                    		{
                    			UUltraToolkitEditorUtility::CleanupMaterialInstanceInvisibleParameters_Impl(Cast<UMaterialInstanceConstant>(SubMenuContext->MaterialEditor.Pin()->GetMaterialInterface()));
                    		}
	                    })),
	                    NAME_None,
	                    EUserInterfaceActionType::Button);

	                MenuBuilder.AddMenuEntry(
	                    LOCTEXT("CleanupMaterialInstanceRedundantOverrideParameters_Label", "清除材质实例冗余覆写参数"),
	                    LOCTEXT("CleanupMaterialInstanceRedundantOverrideParameters_Tip", "清除材质实例冗余覆写参数"),
	                    FSlateIcon(),
	                    FUIAction(FExecuteAction::CreateLambda([&InToolMenuContext]()
	                    {
                    		if (const UMaterialEditorMenuContext* SubMenuContext = InToolMenuContext.FindContext<UMaterialEditorMenuContext>(); SubMenuContext && SubMenuContext->MaterialEditor.IsValid())
                    		{
                    			UUltraToolkitEditorUtility::CleanupMaterialInstanceRedundantOverrideParameters_Impl(Cast<UMaterialInstanceConstant>(SubMenuContext->MaterialEditor.Pin()->GetMaterialInterface()));
                    		}
	                    })),
	                    NAME_None,
	                    EUserInterfaceActionType::Button);
	                MenuBuilder.EndSection();
        		}

        		return MenuBuilder.MakeWidget();
        	}),
        	LOCTEXT("UltraUnrealToolkitButton_Label", "Ultra Toolkit"),
        	LOCTEXT("UltraUnrealToolkitButton_Tip", "Ultra Unreal Toolkit"),
        	FSlateIcon(TEXT("UltraUnrealToolkitStyle"), "UltraUnrealToolkit.Icon_64")
        ));
	}
}

void FUltraUnrealToolkitToolBar::OpenEntryWidget()
{
	const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(FName("AssetRegistry"));
	const IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();
	if (const FAssetData AssetData = AssetRegistry.GetAssetByObjectPath(FSoftObjectPath(TEXT("/UltraUnrealToolkit/EWBP_UltraUnrealToolkit.EWBP_UltraUnrealToolkit"))); AssetData.IsValid())
	{
		if (UEditorUtilityWidgetBlueprint* EditorUtilityWidgetBlueprint = Cast<UEditorUtilityWidgetBlueprint>(AssetData.GetAsset()))
		{
			if (UEditorUtilitySubsystem* EditorUtilitySubsystem = GEditor->GetEditorSubsystem<UEditorUtilitySubsystem>())
			{
				EditorUtilitySubsystem->SpawnAndRegisterTab(EditorUtilityWidgetBlueprint);
			}
		}
	}
}
#undef LOCTEXT_NAMESPACE

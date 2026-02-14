// Copyright Technical Artist - Jiahao.Chan, Individual. All Rights Reserved.

#include "UltraUnrealToolkit.h"

#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleMacros.h"
#include "Styling/SlateStyleRegistry.h"

#include "SecondaryLevel/SecondaryLevelToolMenu.h"
#include "UltraUnrealToolkitToolBar.h"

#define LOCTEXT_NAMESPACE "UltraUnrealToolkitModule"

#define RootToContentDir StyleSet->RootToContentDir

void FUltraUnrealToolkitModule::StartupModule()
{
	FUltraUnrealToolkitToolBar::Startup(this);
	FSecondaryLevelToolMenu::Startup(this);

	const FString PluginDir = IPluginManager::Get().FindPlugin(TEXT("UltraUnrealToolkit"))->GetBaseDir();
	// Set Style
	{
		StyleSet = MakeShared<FSlateStyleSet>("UltraUnrealToolkitStyle");
		StyleSet->SetContentRoot(PluginDir / TEXT("Resources/Slate"));
		StyleSet->SetCoreContentRoot(PluginDir / TEXT("Resources/Slate"));

		const FVector2D Icon8x8(8.0f, 8.0f);
		const FVector2D Icon16x16(16.0f, 16.0f);
		const FVector2D Icon32x32(32.0f, 32.0f);
		const FVector2D Icon40x40(40.0f, 40.0f);
		const FVector2D Icon64x64(64.0f, 64.0f);
		const FVector2D Icon128x128(128.0f, 128.0f);
		const FVector2D Icon256x256(256.0f, 256.0f);

		StyleSet->Set("UltraUnrealToolkit.Icon_40", new IMAGE_BRUSH("Icons/UltraUnrealToolkitIcon_64", Icon40x40));
		StyleSet->Set("UltraUnrealToolkit.Icon_64", new IMAGE_BRUSH("Icons/UltraUnrealToolkitIcon_64", Icon64x64));
		StyleSet->Set("UltraUnrealToolkit.Icon_128", new IMAGE_BRUSH("Icons/UltraUnrealToolkitIcon_128", Icon128x128));
		StyleSet->Set("UltraUnrealToolkit.Icon_256", new IMAGE_BRUSH("Icons/UltraUnrealToolkitIcon_256", Icon256x256));

		FSlateStyleRegistry::RegisterSlateStyle(*StyleSet);
	}
}

void FUltraUnrealToolkitModule::ShutdownModule()
{
	FUltraUnrealToolkitToolBar::Shutdown(this);
	FSecondaryLevelToolMenu::Shutdown(this);
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FUltraUnrealToolkitModule, UltraUnrealToolkit)

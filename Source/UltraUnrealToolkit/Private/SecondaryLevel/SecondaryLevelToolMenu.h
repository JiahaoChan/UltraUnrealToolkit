// Copyright Technical Artist - Jiahao.Chan, Individual. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class FSecondaryLevelToolMenu
{
public:
	static void Startup(void* Owner);

	static void Shutdown(void* Owner);

private:
	static void RegisterMenus();

public:
	static void OnPickLevelClicked();

	static void OnLevelSelected(TArray<FAssetData> LevelAssets);
};

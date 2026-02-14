// Copyright Technical Artist - Jiahao.Chan, Individual. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class FUltraUnrealToolkitToolBar
{
public:
	static void Startup(void* Owner);

	static void Shutdown(void* Owner);

private:
	static void RegisterMenus();

	static void OpenEntryWidget();
};

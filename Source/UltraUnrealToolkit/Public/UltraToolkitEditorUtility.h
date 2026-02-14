// Copyright Technical Artist - Jiahao.Chan, Individual. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Editor/Blutility/Private/EditorFunctionLibrary.h"

#include "UltraToolkitEditorUtility.generated.h"

UCLASS()
class ULTRAUNREALTOOLKIT_API UUltraToolkitEditorUtility : public UEditorFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Ultra Unreal Toolkit")
	static void CleanupMaterialInstanceInvisibleParameters();

	static void CleanupMaterialInstanceInvisibleParameters_Impl(class UMaterialInstanceConstant* MaterialInstance);

	static void CleanupMaterialInstanceRedundantOverrideParameters_Impl(UMaterialInstanceConstant* MaterialInstance);
};

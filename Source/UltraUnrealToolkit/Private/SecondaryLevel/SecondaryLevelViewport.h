// Copyright Technical Artist - Jiahao.Chan, Individual. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SEditorViewport.h"

class SSecondaryLevelViewport : public SEditorViewport
{
public:
	SLATE_BEGIN_ARGS(SSecondaryLevelViewport)
		: _LevelPath( nullptr )
	{
	}
		SLATE_ARGUMENT(FSoftObjectPath, LevelPath)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	virtual ~SSecondaryLevelViewport();

protected:
	//~ Begin SEditorViewport interface
	virtual TSharedRef<FEditorViewportClient> MakeEditorViewportClient() override;
	//~ End SEditorViewport interface

private:
	void OnCopy();

	void OnFocus();

	void OnUnselectAll();

	void TearDownPreviewWorld();

private:
	TSharedPtr<class FSecondaryLevelViewportClient> ViewportClient;

	FSoftObjectPath LevelPath;

	UWorld* SecondaryPreviewWorld = nullptr;
};
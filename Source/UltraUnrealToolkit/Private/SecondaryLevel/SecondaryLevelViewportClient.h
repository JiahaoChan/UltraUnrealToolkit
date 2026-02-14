// Copyright Technical Artist - Jiahao.Chan, Individual. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EditorViewportClient.h"

class FSecondaryLevelViewportClient : public FEditorViewportClient
{
public:
	explicit FSecondaryLevelViewportClient(UWorld* InWorld);

	//~ Begin FViewportClient/FEditorViewportClient interface
	virtual void Tick(float DeltaSeconds) override;

	virtual UWorld* GetWorld() const override { return SecondaryPreviewWorld; }

	virtual bool InputKey(const FInputKeyEventArgs& EventArgs) override;

	virtual void ProcessClick(FSceneView& View, HHitProxy* HitProxy, FKey Key, EInputEvent Event, uint32 HitX, uint32 HitY) override;
	//~ End FViewportClient/FEditorViewportClient interface

	void ProcessCopy();

	void ProcessFocusSelection();

	void ProcessUnselectAllActors();

private:
	static void SetActorSelected(const AActor* Actor, bool bSelected);

private:
	UWorld* SecondaryPreviewWorld;

	TArray<TWeakObjectPtr<AActor>> SelectedActors;
};

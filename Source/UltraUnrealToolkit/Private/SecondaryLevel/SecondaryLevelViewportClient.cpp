// Copyright Technical Artist - Jiahao.Chan, Individual. All Rights Reserved.

#include "SecondaryLevelViewportClient.h"

#include "Editor/UnrealEdEngine.h"
#include "EngineUtils.h"
#include "Engine/World.h"
#include "UnrealEdGlobals.h"
#include "Windows/WindowsPlatformApplicationMisc.h"

FSecondaryLevelViewportClient::FSecondaryLevelViewportClient(UWorld* InWorld)
	: FEditorViewportClient(nullptr)
	, SecondaryPreviewWorld(InWorld)
{
	SetViewLocation(FVector(0,0,300));
	SetViewRotation(FRotator(-20,0,0));
	SetRealtime(true);
	EngineShowFlags.SetSelectionOutline(true);
	EngineShowFlags.SetBuilderBrush(false);
	FEditorViewportClient::SetWidgetMode(UE::Widget::EWidgetMode::WM_None);
	bDisableInput = false;
}

void FSecondaryLevelViewportClient::Tick(float DeltaSeconds)
{
	FEditorViewportClient::Tick(DeltaSeconds);

	if (SecondaryPreviewWorld)
	{
		SecondaryPreviewWorld->Tick(LEVELTICK_All, DeltaSeconds);
	}
}

bool FSecondaryLevelViewportClient::InputKey(const FInputKeyEventArgs& EventArgs)
{
	if (FEditorViewportClient::InputKey(EventArgs))
	{
		return true;
	}
	return false;
}

void FSecondaryLevelViewportClient::ProcessClick(FSceneView& View, HHitProxy* HitProxy, FKey Key, EInputEvent Event, uint32 HitX, uint32 HitY)
{
	if (!Viewport || !SecondaryPreviewWorld || !HitProxy)
	{
		return;
	}

	const FViewportClick Click(&View,this, Key, Event, HitX, HitY);
	const bool bMultiAppend = Click.IsShiftDown() && !Click.IsControlDown();
	const bool bMultiTrim = Click.IsControlDown() && !Click.IsShiftDown();
	const bool bSingleSelect = !bMultiAppend && !bMultiTrim;

	if (HitProxy->IsA(HActor::StaticGetType()))
	{
		const HActor* ActorHitProxy = (HActor*)HitProxy;
		if (AActor* ConsideredActor = ActorHitProxy->Actor)
		{
			while (ConsideredActor->IsChildActor())
			{
				ConsideredActor = ConsideredActor->GetParentActor();
			}

			if (bSingleSelect)
			{
				ProcessUnselectAllActors();
				SetActorSelected(ConsideredActor, true);
				SelectedActors.Emplace(ConsideredActor);
			}
			else
			{
				if (SelectedActors.Contains(ConsideredActor))
				{
					if (bMultiTrim)
					{
						SetActorSelected(ConsideredActor, false);
						SelectedActors.Remove(ConsideredActor);
					}
				}
				else
				{
					SetActorSelected(ConsideredActor, true);
					SelectedActors.Emplace(ConsideredActor);
				}
			}
		}
	}
}

void FSecondaryLevelViewportClient::ProcessCopy()
{
	FString Content;

	TArray<AActor*> Actors;
	TMap<AActor*, FVector> OriginLocations;
	Actors.Reserve(SelectedActors.Num());
	for (const TWeakObjectPtr<AActor>& WeakActor : SelectedActors)
	{
		if (AActor* Actor = WeakActor.Get())
		{
			Actors.Emplace(Actor);
			OriginLocations.Emplace(Actor, Actor->GetActorLocation());
		}
	}
	GUnrealEd->CopyActors(Actors, SecondaryPreviewWorld, &Content);
	// Restore
	for (TPair<AActor*, FVector>& Kvp : OriginLocations)
	{
		if (Kvp.Key)
		{
			Kvp.Key->SetActorLocation(Kvp.Value);
		}
	}

	FPlatformApplicationMisc::ClipboardCopy(*Content);
}

void FSecondaryLevelViewportClient::ProcessFocusSelection()
{
	//GUnrealEd->Exec(SecondaryPreviewWorld, TEXT("CAMERA ALIGN ACTIVEVIEWPORTONLY"));
	if (SelectedActors.Num() == 0)
	{
		return;
	}
	FBox Bounds(ForceInit);
	for (const TWeakObjectPtr<AActor>& WeakActor : SelectedActors)
	{
		if (const AActor* Actor = WeakActor.Get())
		{
			Bounds += Actor->GetComponentsBoundingBox(true);
		}
	}
	FocusViewportOnBox(Bounds);
}

void FSecondaryLevelViewportClient::ProcessUnselectAllActors()
{
	for (const TWeakObjectPtr<AActor>& Actor : SelectedActors)
	{
		SetActorSelected(Actor.Get(), false);
	}
	SelectedActors.Reset();
}

void FSecondaryLevelViewportClient::SetActorSelected(const AActor* Actor, bool bSelected)
{
	if (!Actor)
	{
		return;
	}
	TArray<UPrimitiveComponent*> Components;
	Actor->GetComponents<UPrimitiveComponent>(Components);
	for (const UPrimitiveComponent* Component : Components)
	{
		if (FPrimitiveSceneProxy* SceneProxy = Component ? Component->GetSceneProxy() : nullptr)
		{
			SceneProxy->SetSelection_GameThread(bSelected, false);
		}
	}
}

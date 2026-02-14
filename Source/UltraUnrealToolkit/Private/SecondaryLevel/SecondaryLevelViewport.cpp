// Copyright Technical Artist - Jiahao.Chan, Individual. All Rights Reserved.

#include "SecondaryLevelViewport.h"

#include "EditorViewportCommands.h"
#include "LevelEditorActions.h"
#include "Framework/Commands/GenericCommands.h"

#include "SecondaryLevelViewportClient.h"

void SSecondaryLevelViewport::Construct(const FArguments& InArgs)
{
	LevelPath = InArgs._LevelPath;

	TearDownPreviewWorld();

	UPackage* MapPackage = LoadPackage(nullptr, *LevelPath.ToString(), LOAD_None);
	if (!MapPackage)
	{
		return;
	}
	UWorld* SourceWorld = UWorld::FindWorldInPackage(MapPackage);
	if (!SourceWorld)
	{
		if (!SourceWorld)
		{
			SourceWorld = UWorld::FollowWorldRedirectorInPackage(MapPackage);
			if (SourceWorld)
			{
				MapPackage = SourceWorld->GetOutermost();
			}
		}
	}
	if (!SourceWorld)
	{
		return SEditorViewport::Construct(SEditorViewport::FArguments());
	}

	const FName UniqueName = MakeUniqueObjectName(MapPackage, UWorld::StaticClass(), SourceWorld->GetFName());
	const FString FixedLevelName = MapPackage->GetName() + FString("_PreviewInst_") +UniqueName.ToString();
	const FName FixedLevelFName = FName(*FixedLevelName);
	UWorld::WorldTypePreLoadMap.FindOrAdd(FixedLevelFName) = EWorldType::EditorPreview;
	UPackage* PreviewLevelPackage = CreatePackage(*FixedLevelName);
	PreviewLevelPackage->SetPackageFlags(PKG_NewlyCreated);
	PreviewLevelPackage->SetLoadedPath(MapPackage->GetLoadedPath());
	PreviewLevelPackage->SetSavedHash(MapPackage->GetSavedHash());
	PreviewLevelPackage->MarkAsFullyLoaded();

	FObjectDuplicationParameters Parameters(SourceWorld, PreviewLevelPackage);
	Parameters.DestName = SourceWorld->GetFName();
	Parameters.DestClass = SourceWorld->GetClass();
	Parameters.DuplicateMode = EDuplicateMode::World;
	Parameters.ApplyFlags = RF_Transient;
	UWorld* PreviewWorld = Cast<UWorld>(StaticDuplicateObjectEx(Parameters));
	if (!PreviewWorld)
	{
		return SEditorViewport::Construct(SEditorViewport::FArguments());
	}
	PreviewWorld->ClearFlags(RF_Transactional);

	SecondaryPreviewWorld = PreviewWorld;
	SecondaryPreviewWorld->WorldType = EWorldType::EditorPreview;
	FWorldContext& WorldContext = GEditor->CreateNewWorldContext(EWorldType::EditorPreview);
	WorldContext.SetCurrentWorld(SecondaryPreviewWorld);
	SecondaryPreviewWorld->AddToRoot();
	SecondaryPreviewWorld->InitWorld(UWorld::InitializationValues()
		.InitializeScenes(true)
		.AllowAudioPlayback(false)
		.RequiresHitProxies(true)
		.CreatePhysicsScene(false)
		.CreateNavigation(false)
		.CreateAISystem(false)
		.ShouldSimulatePhysics(false)
		.EnableTraceCollision(false)
		.ForceUseMovementComponentInNonGameWorld(false)
		.SetTransactional(false)
		.CreateFXSystem(true)
		.CreateWorldPartition(true)
		.EnableWorldPartitionStreaming(true));
	SecondaryPreviewWorld->UpdateWorldComponents(true, true);

	SEditorViewport::Construct(SEditorViewport::FArguments());

	CommandList->MapAction(
		FGenericCommands::Get().Copy,
		FExecuteAction::CreateSP(this, &SSecondaryLevelViewport::OnCopy));

	CommandList->MapAction(
		FEditorViewportCommands::Get().FocusViewportToSelection,
		FExecuteAction::CreateSP(this, &SSecondaryLevelViewport::OnFocus));

	CommandList->MapAction(
		FLevelEditorCommands::Get().SelectNone,
		FExecuteAction::CreateSP(this, &SSecondaryLevelViewport::OnUnselectAll));
}

SSecondaryLevelViewport::~SSecondaryLevelViewport()
{
	TearDownPreviewWorld();
}

TSharedRef<FEditorViewportClient> SSecondaryLevelViewport::MakeEditorViewportClient()
{
	ViewportClient = MakeShared<FSecondaryLevelViewportClient>(SecondaryPreviewWorld);
	return ViewportClient.ToSharedRef();
}

void SSecondaryLevelViewport::OnCopy()
{
	if (ViewportClient.IsValid())
	{
		ViewportClient->ProcessCopy();
	}
}

void SSecondaryLevelViewport::OnFocus()
{
	if (ViewportClient.IsValid())
	{
		ViewportClient->ProcessFocusSelection();
	}
}

void SSecondaryLevelViewport::OnUnselectAll()
{
	if (ViewportClient.IsValid())
	{
		ViewportClient->ProcessUnselectAllActors();
	}
}

void SSecondaryLevelViewport::TearDownPreviewWorld()
{
	if (SecondaryPreviewWorld)
	{
		SecondaryPreviewWorld->RemoveFromRoot();
		SecondaryPreviewWorld->DestroyWorld(false);
		GEditor->DestroyWorldContext(SecondaryPreviewWorld);
		SecondaryPreviewWorld = nullptr;
	}
}

// Copyright Technical Artist - Jiahao.Chan, Individual. All Rights Reserved.

#include "UltraToolkitEditorUtility.h"

#include "EditorUtilityLibrary.h"
#include "Engine/SubsurfaceProfile.h"
#include "IMaterialEditor.h"
#include "MaterialEditingLibrary.h"
#include "MaterialEditorUtilities.h"
#include "Materials/MaterialInstanceConstant.h"
#include "StaticParameterSet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(UltraToolkitEditorUtility)

#define LOCTEXT_NAMESPACE "UltraToolkitEditorUtility"

namespace UltraToolkitEditorUtilityImpl
{
	IMaterialEditor* FindMaterialEditorForAsset(UObject* InAsset)
	{
		if (IAssetEditorInstance* AssetEditorInstance = InAsset ? (GEditor ? GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->FindEditorForAsset(InAsset, false) : nullptr) : nullptr)
		{
			// Ensure this is not a UMaterialInstanceDynamic, as that doesn't use IMaterialEditor as its editor
			if (!InAsset->IsA(UMaterialInstanceDynamic::StaticClass()))
			{
				return static_cast<IMaterialEditor*>(AssetEditorInstance);
			}
		}
		return nullptr;
	}
}

void UUltraToolkitEditorUtility::CleanupMaterialInstanceInvisibleParameters()
{
	for (FAssetData& AssetData : UEditorUtilityLibrary::GetSelectedAssetData())
	{
		if (UMaterialInstanceConstant* MaterialInstanceConstant = Cast<UMaterialInstanceConstant>(AssetData.GetAsset()))
		{
			CleanupMaterialInstanceInvisibleParameters_Impl(MaterialInstanceConstant);
		}
	}
}

void UUltraToolkitEditorUtility::CleanupMaterialInstanceInvisibleParameters_Impl(UMaterialInstanceConstant* MaterialInstance)
{
	if (!MaterialInstance || MaterialInstance->IsTemplate())
	{
		return;
	}
	const UMaterial* ParentMaterial = MaterialInstance->GetMaterial();
	if (!ParentMaterial)
	{
		return;
	}

	TArray<FMaterialParameterInfo> VisibleExpressions;
	FMaterialEditorUtilities::GetVisibleMaterialParameters(ParentMaterial, MaterialInstance, VisibleExpressions);

	bool bModified = false;
	FScopedTransaction Transaction(LOCTEXT(__FUNCTION__, "Cleanup Material Instance Invisible Parameters"));

	// Dynamic Parameters - Scalar Parameter
	for (auto It = MaterialInstance->ScalarParameterValues.CreateIterator(); It; ++It)
	{
		if (!VisibleExpressions.Contains(It->ParameterInfo))
		{
			// Skip special parameters.
			FString NameString = It->ParameterInfo.Name.ToString();
			static FName NameSubsurfaceProfile = SubsurfaceProfile::GetSubsurfaceProfileParameterName();
			static FName NameRefractionDepthBias = FName(TEXT("RefractionDepthBias"));
			if (It->ParameterInfo.Name == NameSubsurfaceProfile ||
				NameString.StartsWith(NameSubsurfaceProfile.ToString(), ESearchCase::IgnoreCase) ||
				It->ParameterInfo.Name == NameRefractionDepthBias)
			{
				continue;
			}

			if (!bModified)
			{
				MaterialInstance->Modify();
				bModified = true;
			}
			It.RemoveCurrent();
		}
	}
	// Dynamic Parameters - Vector Parameter
	for (auto It = MaterialInstance->VectorParameterValues.CreateIterator(); It; ++It)
	{
		if (!VisibleExpressions.Contains(It->ParameterInfo))
		{
			// Skip special parameters.
			FString NameString = It->ParameterInfo.Name.ToString();
			if (It->ParameterInfo.Name == NAME_SelectionColor ||
				NameString.StartsWith(FString(TEXT("LayerMask_"), ESearchCase::IgnoreCase)))
			{
				continue;
			}

			if (!bModified)
			{
				MaterialInstance->Modify();
				bModified = true;
			}
			It.RemoveCurrent();
		}
	}
	// Dynamic Parameters - Double Vector Parameter
	for (auto It = MaterialInstance->DoubleVectorParameterValues.CreateIterator(); It; ++It)
	{
		if (!VisibleExpressions.Contains(It->ParameterInfo))
		{
			if (!bModified)
			{
				MaterialInstance->Modify();
				bModified = true;
			}
			It.RemoveCurrent();
		}
	}
	// Dynamic Parameters - Texture Parameter
	for (auto It = MaterialInstance->TextureParameterValues.CreateIterator(); It; ++It)
	{
		if (!VisibleExpressions.Contains(It->ParameterInfo))
		{
			// Skip special parameters.
			FString NameString = It->ParameterInfo.Name.ToString();
			if (It->ParameterInfo.Name == FName(TEXT("WeightmapArray")) ||
				NameString.StartsWith(FString(TEXT("Weightmap"), ESearchCase::IgnoreCase)))
			{
				continue;
			}

			if (!bModified)
			{
				MaterialInstance->Modify();
				bModified = true;
			}
			It.RemoveCurrent();
		}
	}

	// Copy
	FStaticParameterSet StaticParameterSet = MaterialInstance->GetStaticParameters();
	// Static Parameters - Static Switch Parameter
	for (FStaticSwitchParameter& StaticSwitchParameter : StaticParameterSet.StaticSwitchParameters)
	{
		if (!VisibleExpressions.Contains(StaticSwitchParameter.ParameterInfo))
		{
			if (!bModified)
			{
				MaterialInstance->Modify();
				bModified = true;
			}
			// Mark it false and UpdateStaticPermutation() will help us trim.
			StaticSwitchParameter.bOverride = false;
		}
	}
	FStaticParameterSetEditorOnlyData& EditorOnlyStaticParameters = StaticParameterSet.EditorOnly;
	// Static Parameters - Enum Parameter
	for (FStaticEnumParameter& StaticEnumParameter : EditorOnlyStaticParameters.StaticEnumParameters)
	{
		if (!VisibleExpressions.Contains(StaticEnumParameter.ParameterInfo))
		{
			if (!bModified)
			{
				MaterialInstance->Modify();
				bModified = true;
			}
			// Mark it false and UpdateStaticPermutation() will help us trim.
			StaticEnumParameter.bOverride = false;
		}
	}
	// Static Parameters - Component Mask Parameter
	for (FStaticComponentMaskParameter& StaticComponentMaskParameter : EditorOnlyStaticParameters.StaticComponentMaskParameters)
	{
		if (!VisibleExpressions.Contains(StaticComponentMaskParameter.ParameterInfo))
		{
			if (!bModified)
			{
				MaterialInstance->Modify();
				bModified = true;
			}
			// Mark it false and UpdateStaticPermutation() will help us trim.
			StaticComponentMaskParameter.bOverride = false;
		}
	}

	if (bModified)
	{
		MaterialInstance->UpdateStaticPermutation(StaticParameterSet);
		UMaterialEditingLibrary::UpdateMaterialInstance(MaterialInstance);
	}
}

void UUltraToolkitEditorUtility::CleanupMaterialInstanceRedundantOverrideParameters_Impl(UMaterialInstanceConstant* MaterialInstance)
{
	
}
#undef LOCTEXT_NAMESPACE

// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectKR_AssetAction_PropertyExport.h"

#include "EditorUtilityLibrary.h"
#include "JsonObjectConverter.h"
#include "Engine/Blueprint.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

static const TCHAR* ExportDirectory = TEXT(".ExportProperty/");

UProjectKR_AssetAction_PropertyExport::UProjectKR_AssetAction_PropertyExport()
{
	SupportedClasses.Emplace(UObject::StaticClass());
}

void UProjectKR_AssetAction_PropertyExport::ExportObjectToJson()
{
	const TArray<UObject*> SelectedObject_List = UEditorUtilityLibrary::GetSelectedAssets();

	int32 TryExportAssetCount = SelectedObject_List.Num();
	int32 ExportAssetSuccessCount = 0;
	for(UObject* SelectedObject : SelectedObject_List)
	{
		if(SelectedObject == nullptr)
			continue;

		if(const UBlueprint* Blueprint = Cast<UBlueprint>(SelectedObject))
		{
			if(Blueprint->GeneratedClass != nullptr)
			{
				if(ExportObjectToJson(Blueprint->GeneratedClass->GetDefaultObject(), Blueprint->GetName()))
					++ExportAssetSuccessCount;

				continue;
			}
		}

		if(ExportObjectToJson(SelectedObject, SelectedObject->GetName()))
			++ExportAssetSuccessCount;
	}

	const int32 ExportAssetFailCount = TryExportAssetCount - ExportAssetSuccessCount;
	const FString MessageString = FString::Printf(TEXT("UProjectKR_AssetAction_PropertyExport::ExportObjectToJson [Try:%d][Success:%d][Fail:%d]"), TryExportAssetCount, ExportAssetSuccessCount, ExportAssetFailCount);
	FMessageDialog::Open(EAppMsgType::Type::Ok, FText::FromString(MessageString));
}

bool UProjectKR_AssetAction_PropertyExport::ExportObjectToJson(const UObject* InObject, const FString& InFileName) const
{
	if(InObject == nullptr)
		return false;

	FString JsonString = FString();
	if(FJsonObjectConverter::UStructToJsonObjectString(InObject->GetClass(), InObject, JsonString, 0, 0, 0, nullptr, true) == false)
		return false;

	const FString FilePath = FPaths::ProjectDir() + ExportDirectory + InFileName + TEXT(".json");
	return FFileHelper::SaveStringToFile(JsonString, *FilePath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
}

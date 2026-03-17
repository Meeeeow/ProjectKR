// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectKR_ExportPropertyInterface.h"

#include "JsonObjectConverter.h"

static FStringView ExportDirectory = TEXT(".ExportProperty/");

// Add default functionality here for any IProjectKR_ExportPropertyInterface functions that are not pure virtual.
void IProjectKR_ExportPropertyInterface::ExportProperty()
{
	if(const UObject* ExportObject = Cast<UObject>(this))
	{
		const FStringView SaveDirectory = FPaths::ProjectDir() + ExportDirectory;
		const FStringView SaveFileName = ExportObject->GetClass()->GetName(); 

		ExportProperty(SaveDirectory, SaveFileName);
	}
}
void IProjectKR_ExportPropertyInterface::ExportProperty(const FStringView& InSaveDirectory, const FStringView& InSaveName)
{
	if(const UObject* ExportObject = Cast<UObject>(this))
	{
		FString JsonString = FString();
		if(FJsonObjectConverter::UStructToJsonObjectString(ExportObject->GetClass(), ExportObject, JsonString, 0, 0, 0, nullptr, false) == true)
		{
			FString FilePath = FString(InSaveDirectory) + FString(InSaveName) + TEXT(".json");
			FFileHelper::SaveStringToFile(JsonString, *FilePath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
		}
	}
}

// Fill out your copyright notice in the Description page of Project Settings.

#include "SeedExt_DebugPropertyInterface.h"

#include "JsonObjectConverter.h"

static const TCHAR* ExportDirectory = TEXT(".ExportProperty/");

// Add default functionality here for any ISeedExt_DebugPropertyInterface functions that are not pure virtual.
void ISeedExt_DebugPropertyInterface::ExportProperty()
{
	if(const UObject* ExportObject = Cast<UObject>(this))
	{
		if(IsClassDefaultObject(ExportObject) == true)
			return;
		
		const FString SaveDirectory = FPaths::ProjectDir() + ExportDirectory;
		const FString SaveFileName = ExportObject->GetClass()->GetName(); 

		FString JsonString = FString();
		if(FJsonObjectConverter::UStructToJsonObjectString(ExportObject->GetClass(), ExportObject, JsonString, 0, 0, 0, nullptr, true) == true)
		{
			const FString FilePath = FString(SaveDirectory) + FString(SaveFileName) + TEXT(".json");
			FFileHelper::SaveStringToFile(JsonString, *FilePath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
		}
	}
}

void ISeedExt_DebugPropertyInterface::ExportPropertyAdditiveObject(const TArray<UObject*>& InExportPropertyObject_List)
{
	if(const UObject* ExportObject = Cast<UObject>(this))
	{
		if(IsClassDefaultObject(ExportObject) == true)
			return;

		const FString SaveDirectory = FPaths::ProjectDir() + ExportDirectory;
		const FString SaveFileName = ExportObject->GetClass()->GetName();

		FString JsonString = FString();
		if(ExportToJson(ExportObject, JsonString) == false)
			return;

		for(const UObject* InObject : InExportPropertyObject_List)
		{
			if(IsClassDefaultObject(InObject) == true)
				continue;

			FString AdditiveJsonString = FString();
			if(ExportToJson(InObject, AdditiveJsonString) == false)
				continue;

			JsonString += (FString::Printf(TEXT("\n[AdditiveObject:%s]"),*InObject->GetFName().ToString()) + AdditiveJsonString);
		}
		
		const FString FilePath = FString(SaveDirectory) + FString(SaveFileName) + TEXT(".json");
		FFileHelper::SaveStringToFile(JsonString, *FilePath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
	}
}

bool ISeedExt_DebugPropertyInterface::IsClassDefaultObject(const UObject* InObject) const
{
	return InObject->HasAnyFlags(RF_ClassDefaultObject | RF_Transient);
}

bool ISeedExt_DebugPropertyInterface::ExportToJson(const UObject* InObject, FString& OutJsonString)
{
	return FJsonObjectConverter::UStructToJsonObjectString(InObject->GetClass(), InObject, OutJsonString, 0, 0, 0, nullptr, true);
}

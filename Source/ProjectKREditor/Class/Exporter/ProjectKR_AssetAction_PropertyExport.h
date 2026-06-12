// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Editor/Blutility/Classes/AssetActionUtility.h"
#include "ProjectKR_AssetAction_PropertyExport.generated.h"

UCLASS()
class PROJECTKREDITOR_API UProjectKR_AssetAction_PropertyExport : public UAssetActionUtility
{
	GENERATED_BODY()

public:
	UProjectKR_AssetAction_PropertyExport();
	
	UFUNCTION(CallInEditor, Category="ProjectKR|Export")
	void ExportObjectToJson();

private:
	bool ExportObjectToJson(const UObject* InObject, const FString& InFileName) const;
};

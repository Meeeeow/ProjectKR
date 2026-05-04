// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SeedExt_DebugPropertyInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class USeedExt_DebugPropertyInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SEEDEXT_CORE_API ISeedExt_DebugPropertyInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	void ExportProperty();
	void ExportPropertyAdditiveObject(const TArray<UObject*>& InExportPropertyObject_List);

	bool IsClassDefaultObject(const UObject* InObject) const;
	bool ExportToJson(const UObject* InObject, FString& OutJsonString);
};

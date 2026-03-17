// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ProjectKR_ExportPropertyInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UProjectKR_ExportPropertyInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTKR_API IProjectKR_ExportPropertyInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	void ExportProperty();
	void ExportProperty(const FStringView& InSaveDirectory, const FStringView& InSaveName);
};

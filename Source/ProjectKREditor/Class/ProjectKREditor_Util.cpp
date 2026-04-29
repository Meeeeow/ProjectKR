// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectKREditor_Util.h"

FString FProjectKREditor_Util::GetSourcePath()
{
	FString RetVal = FPaths::GetProjectFilePath();

	RetVal += TEXT("Source/ProjectKREditor");
	
	return RetVal;
}

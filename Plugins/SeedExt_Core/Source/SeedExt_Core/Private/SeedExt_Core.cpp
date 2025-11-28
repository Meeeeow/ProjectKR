// Copyright Epic Games, Inc. All Rights Reserved.

#include "SeedExt_Core.h"

#include "SeedExt_Core/SubSystem/SeedExt_SubSystemCollector.h"

#define LOCTEXT_NAMESPACE "FSeedExt_CoreModule"

void FSeedExt_CoreModule::StartupModule()
{
	IModuleInterface::StartupModule();
	
	FSeedExt_SubSystemCollector::_Init();
}

void FSeedExt_CoreModule::ShutdownModule()
{
	FSeedExt_SubSystemCollector::_Reset();

	IModuleInterface::ShutdownModule();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSeedExt_CoreModule, SeedExt_Core)
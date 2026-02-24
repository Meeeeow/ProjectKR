// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectKR_InputSubSystem.h"

#include "EnhancedInputSubsystems.h"

void UProjectKR_InputSubSystem::Init()
{
	Super::Init();

	
}
void UProjectKR_InputSubSystem::Reset()
{
	Super::Reset();
}

void UProjectKR_InputSubSystem::UpdatePlayerController()
{
	
}

TObjectPtr<APlayerController> UProjectKR_InputSubSystem::GetPlayerController()
{
	return PlayerController;
}

void UProjectKR_InputSubSystem::InitEnhancedInput()
{
// #if defined(UE_EDITOR)
// 	pInputMappingContext = UVicExt_BasementResource::_GetObject()->GetInputMappingContext(EVicExt_InputMappintContextType::Editor);
// #elif UE_GAME && PLATFORM_WINDOWS
// 	pInputMappingContext = UVicExt_BasementResource::_GetObject()->GetInputMappingContext(EVicExt_InputMappintContextType::PC);
// #elif UE_GAME && (PLATFORM_ANDROID || PLATFORM_APPLE)
// 	pInputMappingContext = UVicExt_BasementResource::_GetObject()->GetInputMappingContext(EVicExt_InputMappintContextType::Mobile);
// #endif
// 	VICEXT_CHECK(pInputMappingContext != nullptr);
//
// 	if (pPlayerController == nullptr)
// 		return;
//
// 	if (UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pPlayerController->GetLocalPlayer()))
// 	{
// 		SubSystem->AddMappingContext(pInputMappingContext, 0);
// 		pEnhancedPlayerInput = SubSystem->GetPlayerInput();
//
// 		pEnhancedInputUserSettings = SubSystem->GetUserSettings();
// 		if (pEnhancedInputUserSettings == nullptr)
// 			return;
// 		pEnhancedInputUserSettings->RegisterInputMappingContext(pInputMappingContext);
// 	}
	if(UEnhancedInputLocalPlayerSubsystem* EnhancedInputSubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	{
		EnhancedInputSubSystem->AddMappingContext(InputMappingContext, 0);
		PlayerInput = EnhancedInputSubSystem->GetPlayerInput();
		PlayerInputUserSettings = EnhancedInputSubSystem->GetUserSettings();
	}
}
void UProjectKR_InputSubSystem::ResetEnhancedInput()
{
	if(UEnhancedInputLocalPlayerSubsystem* EnhancedInputSubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer())){
		EnhancedInputSubSystem->RemoveMappingContext(InputMappingContext);
		PlayerInput = nullptr;
		PlayerInputUserSettings = nullptr;
	}
}

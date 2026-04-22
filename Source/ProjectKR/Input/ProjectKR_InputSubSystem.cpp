// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectKR_InputSubSystem.h"

#include "EnhancedInputSubsystems.h"

void UProjectKR_InputSubSystem::Init()
{
	Super::Init();

	InitEnhancedInput();
}
void UProjectKR_InputSubSystem::Reset()
{
	ResetEnhancedInput();
	
	Super::Reset();
}
void UProjectKR_InputSubSystem::UpdatePlayerController()
{
	
}

TObjectPtr<APlayerController> UProjectKR_InputSubSystem::GetPlayerController()
{
	if(PlayerController.IsValid() == false)
		return nullptr;
	
	return PlayerController.Get();
}
void UProjectKR_InputSubSystem::SetPlayerController(class APlayerController* InPlayerController)
{
	PlayerController = InPlayerController;
}
const FProjectKR_InputSource* UProjectKR_InputSubSystem::GetPlayingInputConfigSource() const
{
	return InputConfig->GetInputMappingSource(PlayingContextName);
}

void UProjectKR_InputSubSystem::InitEnhancedInput()
{
	if(InputConfig == nullptr)
	{
		const FString InputConfigPath = TEXT("/Game/KR_Input/KR_InputConfig.KR_InputConfig_C");
		if(UClass* GeneratedInputConfig = LoadClass<UProjectKR_InputConfig>(this, *InputConfigPath))
			InputConfig = NewObject<UProjectKR_InputConfig>(this, GeneratedInputConfig, FName(TEXT("InputConfig")));
		
		if(PlayingContextName == NAME_None)
			PlayingContextName = FName(TEXT("Default"));
	}
}
void UProjectKR_InputSubSystem::ResetEnhancedInput()
{

}

UClass* UProjectKR_InputSubSystem::GetSubSystemClass()
{
	return UProjectKR_InputSubSystem::StaticClass();
}
FString UProjectKR_InputSubSystem::GetSubSystemName()
{
	return TEXT("InputSubSystem");
}

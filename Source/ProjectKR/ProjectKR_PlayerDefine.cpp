#include "ProjectKR_PlayerDefine.h"

FString UProjectKR_PlayerStateSaveData::GetSlotName() const
{
	return FString::Format(*UserIdentifierIndex, {UserIdentifier});
}

void UProjectKR_PlayerStateSaveData::SetDefaultValue()
{
	CharacterSoftObjectPath = TEXT("/Script/Engine.Blueprint'/Game/KR_Actor/Character/KR_Player.KR_Player_C'");
}

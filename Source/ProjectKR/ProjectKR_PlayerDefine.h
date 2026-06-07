#pragma once

#include "SeedExt_Core/Delegate/SeedExt_DelegateWrapper.h"
#include "SeedExt_Core/Save/SeedExt_SaveDataBase.h"

#include "ProjectKR_PlayerDefine.generated.h"

DECLARE_MULTICAST_DELEGATE(FProjectKR_OnPlayerLoadComplete);
DECLARE_MULTICAST_DELEGATE(FProjectKR_OnPlayerLoadFail);

class FProjectKR_OnPlayerLoadCompleteDelegator : public FSeedExt_DelegateWrapper<FProjectKR_OnPlayerLoadComplete>
{
public:
	void Broadcast() const { OnDelegate.Broadcast(); }
};
class FProjectKR_OnPlayerLoadFailDelegator : public FSeedExt_DelegateWrapper<FProjectKR_OnPlayerLoadFail>
{
public:
	void Broadcast() const { OnDelegate.Broadcast(); }
};

UCLASS(ClassGroup="ProjectKR")
class UProjectKR_PlayerStateSaveData : public USeedExt_SaveDataBase
{
	GENERATED_BODY()

public:
	virtual FString GetSlotName() const override;
	virtual void SetDefaultValue() override;

	const FSoftObjectPath& GetCharacterSoftObjectPath() const { return CharacterSoftObjectPath; }

private:
	UPROPERTY(SaveGame)
	FSoftObjectPath CharacterSoftObjectPath = FSoftObjectPath();
};
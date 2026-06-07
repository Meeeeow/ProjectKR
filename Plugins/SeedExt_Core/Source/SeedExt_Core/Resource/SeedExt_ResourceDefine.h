#pragma once

#include "CoreMinimal.h"

#include "SeedExt_ResourceDefine.generated.h"

DECLARE_MULTICAST_DELEGATE_ThreeParams(FSeedExt_ResourceLoadCompleteDelegate, FSoftObjectPath, UObject*, UClass*);
DECLARE_MULTICAST_DELEGATE_OneParam(FSeedExt_ResourceLoadFailDelegate, FSoftObjectPath);

UENUM(BlueprintType)
enum class ESeedExt_ResourceLoadOrderType : uint8
{
	None = 0,
	Normal,
	Priority,
	Previous,
	Num					UMETA(Hidden),
};

UENUM(BlueprintType)
enum class ESeedExt_ResourceType : uint8
{
	None = 0,
	Essential			UMETA(DisplayName = "Essential"), 
	Object				UMETA(DisplayName = "Object"),
	Blueprint			UMETA(DisplayName = "Blueprint"),
	Material			UMETA(DisplayName = "Material"),
	Texture				UMETA(DisplayName = "Texture"),
	Mesh				UMETA(DisplayName = "Mesh"),
	SkeletalMesh		UMETA(DisplayName = "SkeletalMesh"),
	Animation			UMETA(DisplayName = "Animation"),
	Particle			UMETA(DisplayName = "Particle"),
	Niagara				UMETA(DisplayName = "Niagara"),
	Sound				UMETA(DisplayName = "Sound"),
	SoundMix			UMETA(DisplayName = "SoundMix"),
	BT					UMETA(DisplayName = "BT"),
	Cinematic			UMETA(DisplayName = "Cinematic"),
	Navigation			UMETA(DisplayName = "Navigation"),
	Widget				UMETA(DisplayName = "Widget"),
	Sprite				UMETA(DisplayName = "Sprite"),
	Font				UMETA(DisplayName = "Font"),
	Data				UMETA(DisplayName = "Data"),
	Level				UMETA(DisplayName = "Level"),
	Editor				UMETA(DisplayName = "Editor"),
	Num					UMETA(Hidden),
};

#define SEEDEXT_RESOURCE_DELAY_DEFAULT			31.0f
#define SEEDEXT_RESOURCE_DELAY_ESSENTIAL		TNumericLimits<float>::Max()
#define SEEDEXT_RESOURCE_DELAY_OBJECT			61.0f
#define SEEDEXT_RESOURCE_DELAY_BLUEPRINT		61.0f
#define SEEDEXT_RESOURCE_DELAY_MATERIAL			11.0f
#define SEEDEXT_RESOURCE_DELAY_TEXTURE			13.0f
#define SEEDEXT_RESOURCE_DELAY_MESH				19.0f
#define SEEDEXT_RESOURCE_DELAY_SKELETALMESH		17.0f
#define SEEDEXT_RESOURCE_DELAY_ANIMATION		31.0f
#define SEEDEXT_RESOURCE_DELAY_PARTICLE			29.0f
#define SEEDEXT_RESOURCE_DELAY_NIAGARA			29.0f
#define SEEDEXT_RESOURCE_DELAY_SOUND			23.0f
#define SEEDEXT_RESOURCE_DELAY_SOUNDMIX			23.0f
#define SEEDEXT_RESOURCE_DELAY_BT				33.0f
#define SEEDEXT_RESOURCE_DELAY_CINEMATIC		53.0f
#define SEEDEXT_RESOURCE_DELAY_NAVIGATION		65.0f
#define SEEDEXT_RESOURCE_DELAY_WIDGET			149.0f
#define SEEDEXT_RESOURCE_DELAY_SPRITE			149.0f
#define SEEDEXT_RESOURCE_DELAY_FONT				151.0f
#define SEEDEXT_RESOURCE_DELAY_DATA				TNumericLimits<float>::Max()
#define SEEDEXT_RESOURCE_DELAY_LEVEL			TNumericLimits<float>::Max()
#define SEEDEXT_RESOURCE_DELAY_EDITOR			TNumericLimits<float>::Max()

class SEEDEXT_CORE_API FSeedExt_ResourceUtil
{
public:
	static bool IsValidResourcePath(const FString& InResourcePath);
	static bool IsValidResourcePath(const FSoftObjectPath& InResourcePath);
	static float GetDefaultUnloadDelay(ESeedExt_ResourceType InType);
};
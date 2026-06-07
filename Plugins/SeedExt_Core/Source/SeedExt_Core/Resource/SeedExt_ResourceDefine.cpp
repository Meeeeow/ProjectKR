#include "SeedExt_ResourceDefine.h"

bool FSeedExt_ResourceUtil::IsValidResourcePath(const FString& InResourcePath)
{
	if(InResourcePath.IsEmpty()==true || InResourcePath==FString())
		return false;

	if(InResourcePath.Contains(TEXT("//"), ESearchCase::CaseSensitive))
		return false;

	if(FPackageName::IsShortPackageName(InResourcePath) == true)
		return false;

	if(FPackageName::IsValidObjectPath(InResourcePath) == false)
		return false;

	return true;
}
bool FSeedExt_ResourceUtil::IsValidResourcePath(const FSoftObjectPath& InResourcePath)
{
	if(InResourcePath.IsValid()==false)
		return false;

	return IsValidResourcePath(InResourcePath.ToString());
}
float FSeedExt_ResourceUtil::GetDefaultUnloadDelay(ESeedExt_ResourceType InType)
{
	switch(InType)
	{
		case ESeedExt_ResourceType::Essential:		return SEEDEXT_RESOURCE_DELAY_ESSENTIAL;
		case ESeedExt_ResourceType::Object:			return SEEDEXT_RESOURCE_DELAY_OBJECT;
		case ESeedExt_ResourceType::Blueprint:		return SEEDEXT_RESOURCE_DELAY_BLUEPRINT;
		case ESeedExt_ResourceType::Material:		return SEEDEXT_RESOURCE_DELAY_MATERIAL;
		case ESeedExt_ResourceType::Texture:		return SEEDEXT_RESOURCE_DELAY_TEXTURE;
		case ESeedExt_ResourceType::Mesh:			return SEEDEXT_RESOURCE_DELAY_MESH;
		case ESeedExt_ResourceType::SkeletalMesh:	return SEEDEXT_RESOURCE_DELAY_SKELETALMESH;
		case ESeedExt_ResourceType::Animation:		return SEEDEXT_RESOURCE_DELAY_ANIMATION;
		case ESeedExt_ResourceType::Particle:		return SEEDEXT_RESOURCE_DELAY_PARTICLE;
		case ESeedExt_ResourceType::Niagara:		return SEEDEXT_RESOURCE_DELAY_NIAGARA;
		case ESeedExt_ResourceType::Sound:			return SEEDEXT_RESOURCE_DELAY_SOUND;
		case ESeedExt_ResourceType::SoundMix:		return SEEDEXT_RESOURCE_DELAY_SOUNDMIX;
		case ESeedExt_ResourceType::BT:				return SEEDEXT_RESOURCE_DELAY_BT;
		case ESeedExt_ResourceType::Cinematic:		return SEEDEXT_RESOURCE_DELAY_CINEMATIC;
		case ESeedExt_ResourceType::Navigation:		return SEEDEXT_RESOURCE_DELAY_NAVIGATION;
		case ESeedExt_ResourceType::Widget:			return SEEDEXT_RESOURCE_DELAY_WIDGET;
		case ESeedExt_ResourceType::Sprite:			return SEEDEXT_RESOURCE_DELAY_SPRITE;
		case ESeedExt_ResourceType::Font:			return SEEDEXT_RESOURCE_DELAY_FONT;
		case ESeedExt_ResourceType::Data:			return SEEDEXT_RESOURCE_DELAY_DATA;
		case ESeedExt_ResourceType::Level:			return SEEDEXT_RESOURCE_DELAY_LEVEL;
		case ESeedExt_ResourceType::Editor:			return SEEDEXT_RESOURCE_DELAY_EDITOR;
		default:									return SEEDEXT_RESOURCE_DELAY_DEFAULT;
	}
}

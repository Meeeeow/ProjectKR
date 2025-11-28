#include "SeedExt_SubSystemInterface.h"
#include "SeedExt_SubSystemCollector.h"

void ISeedExt_SubSystemInterface::Init()
{
	FSeedExt_SubSystemCollector* Collector = FSeedExt_SubSystemCollector::_GetInstance();
	if(Collector == nullptr)
		return;

	if(Collector->AddSubSystem(this) == true){
		SubSystemLoadingState = ESeedExt_LoadingStateType::NotLoading;
		SubSystemPlayingState = ESeedExt_PlayingStateType::NotReady;
	}
}
void ISeedExt_SubSystemInterface::Reset()
{
	FSeedExt_SubSystemCollector* Collector = FSeedExt_SubSystemCollector::_GetInstance();
	if(Collector == nullptr)
		return;

	if(Collector->RemoveSubSystem(this) == true){
		SubSystemLoadingState = ESeedExt_LoadingStateType::NotLoading;
		SubSystemPlayingState = ESeedExt_PlayingStateType::NotReady;
	}
}
void ISeedExt_SubSystemInterface::ClearAll(ESeedExt_ClearReason InClearReason)
{
	
}

bool ISeedExt_SubSystemInterface::IsInitialized()
{
	return true;
}

void ISeedExt_SubSystemInterface::Start()
{
	SubSystemPlayingState = ESeedExt_PlayingStateType::Play; 
}
void ISeedExt_SubSystemInterface::Pause()
{
	SubSystemPlayingState = ESeedExt_PlayingStateType::Pause; 
}
void ISeedExt_SubSystemInterface::Resume()
{
	SubSystemPlayingState = ESeedExt_PlayingStateType::Play;
}
void ISeedExt_SubSystemInterface::Stop()
{
	SubSystemPlayingState = ESeedExt_PlayingStateType::Stop;
}

void ISeedExt_SubSystemInterface::Load()
{
	SubSystemLoadingState = ESeedExt_LoadingStateType::Loading;
}
void ISeedExt_SubSystemInterface::Unload()
{
	SubSystemLoadingState = ESeedExt_LoadingStateType::UnLoading;
}

bool ISeedExt_SubSystemInterface::IsLoaded()
{
	return true;
}
bool ISeedExt_SubSystemInterface::IsUnLoaded()
{
	return true;
}

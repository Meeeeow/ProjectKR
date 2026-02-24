// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectKR_CameraSubSystem.h"

#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"

#include "ProjectKR/Input/ProjectKR_InputSubSystem.h"

#include "SeedExt_Core/SeedExt_Util.h"
#include "SeedExt_Core/SubSystem/SeedExt_SubSystemCollector.h"

bool UProjectKR_CameraSubSystem::Tick_World(float InDeltaSeconds)
{
	//--------------------------------------------------//
	SpawnCameraActor();
	if(GetCameraActor()==nullptr)
		return false;

	UpdatePlayerCameraManager();
	if(GetPlayerCameraManager() == nullptr)
		return false;

	//--------------------------------------------------//
	
	return Super::Tick_World(InDeltaSeconds);
}

void UProjectKR_CameraSubSystem::SpawnCameraActor()
{
	if(CameraActor != nullptr)
		return;

	CameraActor = SeedExt_SpawnActor<ACameraActor>(GetSubSystemWorld(), ACameraActor::StaticClass(), FVector::ZeroVector,FRotator::ZeroRotator,FVector::OneVector, TEXT("ProjectKR_CameraActor"), true);
	check(CameraActor != nullptr);
	if(UCameraComponent* CameraComponent = CameraActor->GetCameraComponent(); IsValid(CameraComponent)==true)
		CameraComponent->SetConstraintAspectRatio(false);
	
#if WITH_EDITOR
	//CameraActor->GetRootComponent()->AttachToComponent(FSeedExt_SubSystemCollector::_GetInstance()->GetSubsystemActor(this)->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
#endif

	
}
void UProjectKR_CameraSubSystem::DespawnCameraActor()
{
	
}

void UProjectKR_CameraSubSystem::UpdatePlayerCameraManager()
{
	if(GetPlayerCameraManager() != nullptr)
		return;

	UProjectKR_InputSubSystem* pInputManager = ProjectKR_GetInputManager(GetSubSystemWorld());
	if(pInputManager!=nullptr && pInputManager->GetPlayerController()!=nullptr && pInputManager->GetPlayerController()->PlayerCameraManager!=nullptr){
		PlayerCameraManager = pInputManager->GetPlayerController()->PlayerCameraManager;
		PlayerCameraManager->SetViewTarget(GetCameraActor());
	}
	else
		PlayerCameraManager = nullptr;
}

// void UVicExt_CameraManager::UpdateCamera(float fDeltaSeconds)
// {
// 	if(VicExt_IsPlaying(eCameraState) == false)
// 		return;
//
// 	if(pCameraConfig == nullptr)
// 		return;
//
// 	FVicExt_CameraValue oBackupCurrentCameraValue = oCurrentCameraValue;
//
// 	{
// 		if(pCameraConfig->fInputCameraValue_ResetFactor!=VICEXT_NONEVALUE
// 		//&& VicExt_GetInputManager(GetSubSystemWorld())->GetAxisInputDuration(EVicExt_Axis_InputEvent::LookUpDown)==0.0f
// 		//&& VicExt_GetInputManager(GetSubSystemWorld())->GetAxisInputDuration(EVicExt_Axis_InputEvent::LookRightLeft)==0.0f
// //-->wtlee Temporay : EnhnacedInput : Axis Integrate
// && VicExt_GetInputManager(GetSubSystemWorld())->GetAxisInputDuration(EVicExt_Axis_InputEvent::LookAt)==0.0f
// //-->wtlee Temporay : EnhnacedInput : Axis Integrate
// 		&& VicExt_GetInputManager(GetSubSystemWorld())->GetAxisInputDuration(EVicExt_Axis_InputEvent::LookZoom)==0.0f){
// 			FVicExt_CameraValue oResetInputCameraValue = FVicExt_CameraValue::None;
// 			oResetInputCameraValue.fDistance = oDelayedInputCameraValue.fDistance;
// 			oResetInputCameraValue.fFov = oDelayedInputCameraValue.fFov;
// 			FVicExt_CameraUtil::InterpolateCameraValue(oDelayedInputCameraValue,oResetInputCameraValue,fDeltaSeconds*pCameraConfig->fInputCameraValue_ResetFactor);	//Input to Reset
// 		}
//
// 		FVicExt_CameraUtil::InterpolateCameraValue(oInputCameraValue, oDelayedInputCameraValue, fDeltaSeconds,
// 													FVicExt_ConfigValue_Camera::CameraDelayFactor_Input_Position(),
// 													FVicExt_ConfigValue_Camera::CameraDelayFactor_Input_Rotation(),
// 													FVicExt_ConfigValue_Camera::CameraDelayFactor_Input_Distance(),
// 													FVicExt_ConfigValue_Camera::CameraDelayFactor_Input_Fov());
//
// 		FVicExt_CameraUtil::InterpolateCameraValue(oAdditiveCameraValue, oDelayedAdditiveCameraValue, fDeltaSeconds,
// 													FVicExt_ConfigValue_Camera::CameraDelayFactor_Additive_Position(),
// 													FVicExt_ConfigValue_Camera::CameraDelayFactor_Additive_Rotation(),
// 													FVicExt_ConfigValue_Camera::CameraDelayFactor_Additive_Distance(),
// 													FVicExt_ConfigValue_Camera::CameraDelayFactor_Additive_Fov());
// 	}
// 	FVicExt_CameraValue oAddedCameraValue = oInputCameraValue + oAdditiveCameraValue;
// 	if(bApplyCameaAxis_Input==true || bApplyCameaAxis_Additive==true){
// 		FVector vTargetPosition_Input = (bApplyCameaAxis_Input==true)?VicExt_GetRotatedVector(GetCameraRotation(),oInputCameraValue.vTargetPosition):oInputCameraValue.vTargetPosition;
// 		FVector vTargetPosition_Additive = (bApplyCameaAxis_Additive==true)?VicExt_GetRotatedVector(GetCameraRotation(), oAdditiveCameraValue.vTargetPosition):oAdditiveCameraValue.vTargetPosition;
// 		oAddedCameraValue.vTargetPosition = vTargetPosition_Input + vTargetPosition_Additive;
//
// 		//FVector vPosition_Input = (bApplyCameaAxis_Input==true)?VicExt_GetRotatedVector(GetCameraRotation(), oInputCameraValue.vPosition):oInputCameraValue.vPosition;
// 		//FVector vPosition_Additive = (bApplyCameaAxis_Additive==true)?VicExt_GetRotatedVector(GetCameraRotation(), oAdditiveCameraValue.vPosition):oAdditiveCameraValue.vPosition;
// 		//oAddedCameraValue.vPosition = vPosition_Input + vPosition_Additive;
// 	}
//
// 	//--------------------------------------------------// Update PurposeCamera Value
// 	{
// 		//FVicExt_CameraValue oPurposeCameraValue_Previous;
// 		//pCameraConfig_Previous->ApplyCameraConfig(GetSubSystemWorld(), oPurposeCameraValue_Previous);
// 		//FVicExt_CameraValue oPurposeCameraValue_Next;
// 		//pCameraConfig_Next->ApplyCameraConfig(GetSubSystemWorld(), oPurposeCameraValue_Next);
// 		//FVicExt_CameraValue oPurposeCameraValue = FMath::Lerp(oPurposeCameraValue_Previous,oPurposeCameraValue_Next,fFactor);
//
// 		FVector vFollowPawnAddedHeight = FVector(0.0f,0.0f,fFollowPawnAddedHeight);
// 		if(pFollowPawn.IsValid()==true && sFollowPawnBoneName.IsNone()==false){
// 			FVector vBonePosition = FVicExt_PawnUtil::GetPawnSocketPosition(VicExt_GetValidObject(pFollowPawn), EVicExt_PawnPartsType::Skeletal_Main,sFollowPawnBoneName, ERelativeTransformSpace::RTS_World);
// 			vFollowPawnAddedHeight = vBonePosition - pFollowPawn->GetActorLocation();
// 		}
// 		FVector vTargetPawnAddedHeight = FVector(0.0f,0.0f,fTargetPawnAddedHeight);
// 		if(pTargetPawn.IsValid()==true && sTargetPawnBoneName.IsNone()==false){
// 			FVector vBonePosition = FVicExt_PawnUtil::GetPawnSocketPosition(VicExt_GetValidObject(pTargetPawn), EVicExt_PawnPartsType::Skeletal_Main,sTargetPawnBoneName, ERelativeTransformSpace::RTS_World);
// 			vTargetPawnAddedHeight = vBonePosition - pTargetPawn->GetActorLocation();
// 		}
// 		pCameraConfig->ApplyCameraConfig(GetSubSystemWorld(), oPurposeCameraValue, VicExt_GetValidObject(pFollowPawn),vFollowPawnAddedHeight, VicExt_GetValidObject(pTargetPawn),vTargetPawnAddedHeight, oAddedCameraValue);
//
// 		//--------------------------------------------------//
// 		const FVector vCurrentCameraPosition = GetCameraPosition();
// 		const float fCameraAnglesOffsetCheckLimit = 10.0f;	//카메라 변화가 있을때만 계산, 카메라 고정 중 일때는 체크할 필요가 없다.
// 		if((vCurrentCameraPosition-oPurposeCameraValue.vPosition).Size() > fCameraAnglesOffsetCheckLimit){
// 			//if( oPurposeCameraValue.vPosition.Equals(vCurrentCameraPosition) == false )
// 				pCameraConfig->ApplyCameraOffset(GetSubSystemWorld(), oPurposeCameraValue, vCurrentCameraPosition, oAddedCameraValue);
// 		}
//
// 		//--------------------------------------------------//
// 		if(pCameraConfig->IsCameraLight() == true){
// 			if(pCameraConfigLight == nullptr){
// 				pCameraConfigLight = VicExt_GetPoolUObject(UVicExt_Container);
// 				pCameraConfigLight->Init();
// 			}
// 			VICEXT_CHECK(pCameraConfigLight != nullptr);
// 			pCameraConfig->ApplyCameraLight(GetSubSystemWorld(), pCameraConfigLight);
// 			pCameraConfig->ApplyCameraEnvironmentStatus(GetSubSystemWorld(), pCameraConfigLight);
// 		}
//
// 		//--------------------------------------------------//
// 		if(pCameraConfig->IsCameraExtra() == true){
// 			if(pCameraConfigExtra == nullptr){
// 				pCameraConfigExtra = VicExt_GetPoolUObject(UVicExt_Container);
// 				pCameraConfigExtra->Init();
// 			}
// 			VICEXT_CHECK(pCameraConfigExtra != nullptr);
// 			pCameraConfig->ApplyCameraExtra(GetSubSystemWorld(), pCameraConfigExtra, oPurposeCameraValue, oAddedCameraValue);
// 		}
// 	}
//
// 	//--------------------------------------------------// Update TotalShakerCamera Value
// 	oTotalShakerCameraValue = FVicExt_CameraValue::None;
// 	for(TMap<FVicExt_ShakerHandle,FVicExt_CameraValue>::TIterator It(oShakerCameraValue_List); It; ++It)
// 		oTotalShakerCameraValue = oTotalShakerCameraValue + It.Value();
// 	
// 	//--------------------------------------------------// Update CurrentCamera Value
// 	FVicExt_CameraUtil::InterpolateCameraValue(oCurrentCameraValue, oPurposeCameraValue, fDeltaSeconds,
// 											pCameraConfig->oValue_Base.fCameraDelayFactor_Position,pCameraConfig->oValue_Base.fCameraDelayFactor_Rotation,pCameraConfig->oValue_Base.fCameraDelayFactor_Distance,pCameraConfig->oValue_Base.fCameraDelayFactor_Fov);
//
// 	if(pCameraConfig->bCheckCollision == true)
// 	{
// 		FVicExt_CameraValue oCheckCameraValue;
// 		{
// 			oCheckCameraValue.vLocationOffset = oCurrentCameraValue.vLocationOffset+oAddedCameraValue.vLocationOffset;
// 			oCheckCameraValue.rAnglesOffset = oCurrentCameraValue.rAnglesOffset+oAddedCameraValue.rAnglesOffset;
//
// 			FRotator rResultCameraAngles = oCurrentCameraValue.rAngles + oAddedCameraValue.rAngles;
// 			FVector vResultCameraForward = VicExt_GetRotatedVector(rResultCameraAngles+oCheckCameraValue.rAnglesOffset,-FVector::ForwardVector);
// 			oCheckCameraValue.fFov = oCurrentCameraValue.fFov + oAddedCameraValue.fFov;
// 			oCheckCameraValue.fDistance = oCurrentCameraValue.fDistance + oAddedCameraValue.fDistance;
// 			oCheckCameraValue.rAngles = rResultCameraAngles + oCheckCameraValue.rAnglesOffset;
// 			oCheckCameraValue.vTargetPosition = oCurrentCameraValue.vTargetPosition + oAddedCameraValue.vTargetPosition;
// 			//oCheckCameraValue.vPosition = oCheckCameraValue.vTargetPosition + vResultCameraForward*(oCheckCameraValue.fDistance) + VicExt_GetRotatedVector(rResultCameraAngles,oCheckCameraValue.vLocationOffset) + oAddedCameraValue.vPosition;
// 			oCheckCameraValue.vPosition = oCheckCameraValue.vTargetPosition + vResultCameraForward*(oCheckCameraValue.fDistance) + VicExt_GetRotatedVector(rResultCameraAngles,oCheckCameraValue.vLocationOffset);
// 		}
//
// 		FVicExt_CameraValue oCorrectedCameraValue;
// 		bool bIsCollided = FVicExt_CameraUtil::ApplyCollisionCorrect(GetSubSystemWorld(), oCorrectedCameraValue, oCheckCameraValue);
// 		if(bIsCollided==true || FVicExt_CameraValue::IsNearlyEqual(oCollisionCorrectCameraValue,oCorrectedCameraValue)==false)
// 		{
// 			float fCameraDelayMultifly = (bIsCollided==true) ? FVicExt_ConfigValue_Camera::CameraDelayFactor_Multifly_CollisionIn() : FVicExt_ConfigValue_Camera::CameraDelayFactor_Multifly_CollisionOut();
// 			FVicExt_CameraUtil::InterpolateCameraValue(oCollisionCorrectCameraValue, oCorrectedCameraValue, fDeltaSeconds,
// 														FVicExt_ConfigValue_Camera::CameraDelayFactor_Default_Position()*fCameraDelayMultifly,
// 														FVicExt_ConfigValue_Camera::CameraDelayFactor_Default_Rotation()*fCameraDelayMultifly,
// 														FVicExt_ConfigValue_Camera::CameraDelayFactor_Default_Distance()*fCameraDelayMultifly,
// 														FVicExt_ConfigValue_Camera::CameraDelayFactor_Default_Fov()*fCameraDelayMultifly);
//
// 			oAddedCameraValue = oAddedCameraValue + oCollisionCorrectCameraValue;
// 		}
// 	}
//
// 	//--------------------------------------------------// Apply CurrentCamera Value
// 	{
// 		//Addtive,Shaker를 적용한 최종값.
// 		FVicExt_CameraValue oResultCameraValue;
// 		{
// 			oResultCameraValue.vLocationOffset = oCurrentCameraValue.vLocationOffset+oAddedCameraValue.vLocationOffset;
// 			oResultCameraValue.rAnglesOffset = oCurrentCameraValue.rAnglesOffset+oAddedCameraValue.rAnglesOffset;
//
// 			FRotator rResultCameraAngles = oCurrentCameraValue.rAngles + oAddedCameraValue.rAngles + oTotalShakerCameraValue.rAngles;
// 			FVector vResultCameraForward = VicExt_GetRotatedVector(rResultCameraAngles+oResultCameraValue.rAnglesOffset,-FVector::ForwardVector);
// 			oResultCameraValue.fFov = oCurrentCameraValue.fFov + oAddedCameraValue.fFov + oTotalShakerCameraValue.fFov;
// 			oResultCameraValue.fDistance = oCurrentCameraValue.fDistance + oAddedCameraValue.fDistance + oTotalShakerCameraValue.fDistance;
// 			oResultCameraValue.rAngles = rResultCameraAngles + oResultCameraValue.rAnglesOffset;
// 			oResultCameraValue.vTargetPosition = oCurrentCameraValue.vTargetPosition + oAddedCameraValue.vTargetPosition + oTotalShakerCameraValue.vTargetPosition;
// 			//oResultCameraValue.vPosition = oResultCameraValue.vTargetPosition + vResultCameraForward*(oResultCameraValue.fDistance) + VicExt_GetRotatedVector(rResultCameraAngles,oResultCameraValue.vLocationOffset) + oAddedCameraValue.vPosition + oTotalShakerCameraValue.vPosition;
// 			oResultCameraValue.vPosition = oResultCameraValue.vTargetPosition + vResultCameraForward*(oResultCameraValue.fDistance) + VicExt_GetRotatedVector(rResultCameraAngles,oResultCameraValue.vLocationOffset) + oTotalShakerCameraValue.vPosition;
// 		}
// 		//~Addtive,Shaker를 적용한 최종값.
//
// 		oLastCameraValue = oResultCameraValue;
// 		oLastDiffCameraValue = oBackupCurrentCameraValue - oCurrentCameraValue;
// 		//VICEXT_LOG(TEXT("UVicExt_CameraManager::UpdateCamera(%d) : LastCameraValue : TargetPosition=%s\tAngles=%s\tDistance=%f\tFov=%f"), GFrameNumber,*oLastCameraValue.vTargetPosition.ToString(),*oLastCameraValue.rAngles.ToString(),oLastCameraValue.fDistance,oLastCameraValue.fFov);
// 	}
// }

void UProjectKR_CameraSubSystem::SetExternalCameraActor(ACameraActor* pInExternalCameraActor)
{
	
}

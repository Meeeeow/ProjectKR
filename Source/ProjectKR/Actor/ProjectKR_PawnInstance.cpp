// Fill out your copyright notice in the Description page of Project Settings.
#include "ProjectKR_PawnInstance.h"

#include "Components/CapsuleComponent.h"

//#include "Movement/ProjectKR_MovementComponent.h"

AProjectKR_PawnInstance::AProjectKR_PawnInstance(const FObjectInitializer& ObjectInitializer)	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	{
		static FName RootSceneComponentName(TEXT("ProjectKR_RootSceneComponent"));
		RootSceneComponent = CreateDefaultSubobject<USceneComponent>(RootSceneComponentName);
		RootSceneComponent->Mobility = EComponentMobility::Movable;
		RootSceneComponent->Activate(true);
		RootComponent = RootSceneComponent;
		check(RootSceneComponent != nullptr);
	}
	{
		//static FName MovementComponentName(TEXT("ProjectKR_MovementComponent"));
		//MovementComponent = CreateDefaultSubobject<UProjectKR_MovementComponent>(MovementComponentName);
		//check(MovementComponent != nullptr);
		//MovementComponent->Activate(true);
	}
	{
		static FName CollisionShapeComponentName(TEXT("ProjectKR_CollisionShape"));
		{
			UCapsuleComponent* CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(CollisionShapeComponentName);
			check(CapsuleComponent != nullptr);
			CapsuleComponent->InitCapsuleSize(50.f,90.f);
			CapsuleComponent->SetRelativeLocation(FVector(0.0f,0.0f,90.0f));
			ShapeComponent = CapsuleComponent;
		}
		check(ShapeComponent != nullptr);
		ShapeComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
		ShapeComponent->SetCollisionResponseToChannel(ECC_WorldStatic,ECR_Ignore);
		ShapeComponent->CanCharacterStepUpOn = ECB_Yes;
		ShapeComponent->bDynamicObstacle = false;
		ShapeComponent->SetEnableGravity(false);
		ShapeComponent->SetCanEverAffectNavigation(false);
		ShapeComponent->Activate(true);
		ShapeComponent->SetupAttachment(RootSceneComponent);
	}
	{
		static FName SkeletalMeshComponentName(TEXT("ProjectKR_SkeletalMeshComponent"));
		SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(SkeletalMeshComponentName);
		check(SkeletalMeshComponent != nullptr);

		if(USkeletalMesh* SkeletalMeshObject = LoadObject<USkeletalMesh>(this, TEXT("/Script/Engine.SkeletalMesh'/Game/KoreanTraditionalMartialArts/Meshs/Characters/Meshs/SKM_Soldier_1.SKM_Soldier_1'")))
			SkeletalMeshComponent->SetSkeletalMesh(SkeletalMeshObject);
		
		SkeletalMeshComponent->SetupAttachment(RootSceneComponent);
	}
}

//TObjectPtr<UProjectKR_MovementComponent> AProjectKR_PawnInstance::GetMovementComponentOverride()
//{
//	return MovementComponent;
//`}

void AProjectKR_PawnInstance::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AProjectKR_PawnInstance::Init()
{
	PawnPlayingState = ESeedExt_PlayingStateType::None;

	//--------------------------------------------------//

}
void AProjectKR_PawnInstance::ClearAll()
{
}
void AProjectKR_PawnInstance::Reset()
{
	Super::Reset();
}

void AProjectKR_PawnInstance::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);
}

void AProjectKR_PawnInstance::BeginPlay()
{
	Super::BeginPlay();
}
void AProjectKR_PawnInstance::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}
void AProjectKR_PawnInstance::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AProjectKR_PawnInstance::SetPawnBP(class UBlueprintGeneratedClass* InBlueprintGeneratedClass)
{
	
}
void AProjectKR_PawnInstance::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
}


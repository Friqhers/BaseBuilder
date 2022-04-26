// Fill out your copyright notice in the Description page of Project Settings.


#include "BBBaseBlock.h"
#include "Camera/CameraComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetMathLibrary.h"
#include "BBCharacter.h"

// Sets default values
ABBBaseBlock::ABBBaseBlock()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BaseBlockMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = BaseBlockMesh;

	//SetReplicates(true);
	bReplicates = true;
}

// Called when the game starts or when spawned
void ABBBaseBlock::BeginPlay()
{
	Super::BeginPlay();
	SetReplicateMovement(true);
	lastPosition = GetActorLocation();

	DynamicMaterial = UMaterialInstanceDynamic::Create(Material, this);
}

// Called every frame
void ABBBaseBlock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	//@TODO: fix this, working but laggy on client
	// if(HasAuthority())
	// {
	// 	if(BlockIsActive && OwnerCharacter)
	// 	{
	// 		UpdatePosition();
	// 	}
	// }
	
	if(BlockIsActive && OwnerCharacter)
	{
		
		if(HasAuthority())
		{
			//Cast<APlayerController>(OwnerCharacter->Controller)->SmoothTargetViewRotation(OwnerCharacter, DeltaTime);
			UpdatePosition();
		}
		else
		{
			if(OwnerCharacter->IsLocallyControlled())
			{
				UpdatePosition();
			}
			else
			{
				
				SetActorLocation(FMath::VInterpTo(lastPosition, blockTeleportPosition, DeltaTime, 10));
			}
			//ServerUpdatePosition();
			
		}

		lastPosition = GetActorLocation();
	}
}

void ABBBaseBlock::Lock(ABBCharacter* ownerPlayer)
{
	if(ownerPlayer)
	{
		OwnerCharacter = ownerPlayer;
		BlockIsLockedToOwner = true;

		//set paramater with Set***ParamaterValue
		DynamicMaterial->SetVectorParameterValue("Color", OwnerCharacter->blockColor);
		BaseBlockMesh->SetMaterial(0, DynamicMaterial);
	}
}

void ABBBaseBlock::Unlock()
{
	if(BlockIsLockedToOwner)
	{
		BlockIsLockedToOwner = false;
		OwnerCharacter = nullptr;

		//set paramater with Set***ParamaterValue
		DynamicMaterial->SetVectorParameterValue("Color", FVector(0.082353, 0.07451, 0.07451));
		BaseBlockMesh->SetMaterial(0, DynamicMaterial);
	}
}

void ABBBaseBlock::OnRep_BlockLockChanges()
{
	if(BlockIsLockedToOwner)
	{
		DynamicMaterial->SetVectorParameterValue("Color", OwnerCharacter->blockColor);
		BaseBlockMesh->SetMaterial(0, DynamicMaterial);
	}
	else
	{
		DynamicMaterial->SetVectorParameterValue("Color", FVector(0.082353, 0.07451, 0.07451));
		BaseBlockMesh->SetMaterial(0, DynamicMaterial);
	}
}

void ABBBaseBlock::OnRep_BlockIsActive()
{
	if(BlockIsActive)
	{
		//DynamicMaterial->BlendMode = EBlendMode::BLEND_Translucent;
		DynamicMaterial->SetScalarParameterValue("Opacity", 0.5f);
		BaseBlockMesh->SetMaterial(0, DynamicMaterial);
	}
	else
	{
		//DynamicMaterial->BlendMode = EBlendMode::BLEND_Opaque;
		DynamicMaterial->SetScalarParameterValue("Opacity", 1.0f);
		BaseBlockMesh->SetMaterial(0, DynamicMaterial);
	}
}


void ABBBaseBlock::UpdatePosition()
{
	FVector eyeLocation;
	FRotator eyeRotation;
	OwnerCharacter->GetActorEyesViewPoint(eyeLocation, eyeRotation);
	FVector direction;

	direction= eyeRotation.Vector();
	
	blockTeleportPosition = (eyeLocation + (direction * OwnerCharacter->distanceBetween)) - OwnerCharacter->blockOffset;

	SetActorLocation(blockTeleportPosition);
}

void ABBBaseBlock::ServerUpdatePosition_Implementation()
{
	UpdatePosition();
}


void ABBBaseBlock::OnRep_CollisionEnabled()
{
	SetActorEnableCollision(collisionEnabled);
}

void ABBBaseBlock::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABBBaseBlock, BlockIsActive);
	DOREPLIFETIME(ABBBaseBlock, OwnerCharacter);
	DOREPLIFETIME(ABBBaseBlock, collisionEnabled);
	DOREPLIFETIME(ABBBaseBlock, blockTeleportPosition);
	DOREPLIFETIME(ABBBaseBlock, BlockIsLockedToOwner);
}

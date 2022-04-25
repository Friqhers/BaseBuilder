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
	DOREPLIFETIME(ABBBaseBlock,collisionEnabled);
	DOREPLIFETIME(ABBBaseBlock, blockTeleportPosition);
}

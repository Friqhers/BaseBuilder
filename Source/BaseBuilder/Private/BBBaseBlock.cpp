// Fill out your copyright notice in the Description page of Project Settings.


#include "BBBaseBlock.h"
#include "BBCharacter.h"
#include "Camera/CameraComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ABBBaseBlock::ABBBaseBlock()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BaseBlockMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = BaseBlockMesh;

	SetReplicates(true);
}

// Called when the game starts or when spawned
void ABBBaseBlock::BeginPlay()
{
	Super::BeginPlay();
	SetReplicateMovement(true);
}

// Called every frame
void ABBBaseBlock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	//@TODO: fix this, working but laggy on client
	if(HasAuthority())
	{
		if(BlockIsActive && OwnerCharacter)
		{
			UpdatePosition();
		}
	}

	// if(BlockIsActive && OwnerCharacter)
	// {
	// 	UpdatePosition();
	//
	// 	if(HasAuthority() == false)
	// 	{
	// 		ServerUpdatePosition();
	// 	}
	// }

	
	// else
	// {
	// 	if(BlockIsActive && OwnerCharacter)
	// 	{
	// 		FVector eyeLocation;
	// 		FRotator eyeRotation;
	// 		OwnerCharacter->GetActorEyesViewPoint(eyeLocation, eyeRotation);
	//
	// 		FVector direction = eyeRotation.Vector();
	// 		FVector blockTeleportPosition = eyeLocation + (direction * OwnerCharacter->distanceBetween);
	//
	// 		SetActorLocation(FMath::VInterpTo(GetActorLocation(), blockTeleportPosition, DeltaTime, 50));
	// 		ServerUpdatePosition_Implementation(GetActorLocation(), blockTeleportPosition);
	// 	}
	// }
}

void ABBBaseBlock::UpdatePosition()
{
	FVector eyeLocation;
	FRotator eyeRotation;
	OwnerCharacter->GetActorEyesViewPoint(eyeLocation, eyeRotation);

	FVector direction = eyeRotation.Vector();
	FVector blockTeleportPosition = eyeLocation + (direction * OwnerCharacter->distanceBetween);

	SetActorLocation(blockTeleportPosition - OwnerCharacter->blockOffset);
}

void ABBBaseBlock::ServerUpdatePosition_Implementation()
{
	UpdatePosition();
}


void ABBBaseBlock::OnRep_CollisionEnabled()
{
	
}

void ABBBaseBlock::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABBBaseBlock, BlockIsActive);
	DOREPLIFETIME(ABBBaseBlock, OwnerCharacter);
	DOREPLIFETIME(ABBBaseBlock,collisionEnabled);
}

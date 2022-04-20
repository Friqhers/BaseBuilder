// Fill out your copyright notice in the Description page of Project Settings.


#include "BBBaseBlock.h"

#include "BBCharacter.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ABBBaseBlock::ABBBaseBlock()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BaseBlockMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = BaseBlockMesh;

}

// Called when the game starts or when spawned
void ABBBaseBlock::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ABBBaseBlock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (BlockIsActive && OwnerCharacter)
	{
		UpdatePosition();
		if (!HasAuthority())
		{
			ServerUpdatePosition();
		}
	}
}

void ABBBaseBlock::UpdatePosition()
{
	FVector eyeLocation;
	FRotator eyeRotation;
	OwnerCharacter->GetActorEyesViewPoint(eyeLocation, eyeRotation);

	FVector direction = eyeRotation.Vector();
	FVector blockTeleportPosition = eyeLocation + (direction * defaultDistanceBetween);

	SetActorLocation(blockTeleportPosition);

	if(HasAuthority())
	{
		CurrentObjectAndMovingMode.NewMovableObject = Cast<AActor>(this);
		CurrentObjectAndMovingMode.TeleportDest = blockTeleportPosition;
	}
}

void ABBBaseBlock::ServerUpdatePosition_Implementation()
{
	UpdatePosition();
}

bool ABBBaseBlock::ServerUpdatePosition_Validate()
{
	return true;
}


void ABBBaseBlock::OnRep_ObjectMovingChange()
{
	CurrentObjectAndMovingMode.NewMovableObject->SetActorLocation(CurrentObjectAndMovingMode.TeleportDest);
	CurrentObjectAndMovingMode.NewMovableObject = nullptr;
	CurrentObjectAndMovingMode.TeleportDest = FVector();
}


void ABBBaseBlock::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABBBaseBlock, BlockIsActive);
	DOREPLIFETIME(ABBBaseBlock, OwnerCharacter);
	DOREPLIFETIME(ABBBaseBlock, CurrentObjectAndMovingMode);
}

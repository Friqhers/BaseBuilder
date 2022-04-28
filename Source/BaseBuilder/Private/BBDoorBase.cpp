// Fill out your copyright notice in the Description page of Project Settings.


#include "BBDoorBase.h"

// Sets default values
ABBDoorBase::ABBDoorBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	OpenTime = 1.0f;
	CloseTime = 1.0f;

	bReplicates = true;
	DoorState = EDoorState::Closed;

}

void ABBDoorBase::Reset()
{
	Super::Reset();
	CheckBeforeClosing();
}

// Called when the game starts or when spawned
void ABBDoorBase::BeginPlay()
{
	Super::BeginPlay();
	SetReplicateMovement(true);

}

// Called every frame
void ABBDoorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABBDoorBase::CheckBeforeOpening()
{
	if (DoorState == EDoorState::Closed)
	{
		DoorState = EDoorState::Opening;
		OpenDoor();
		
	}
	else if (DoorState == EDoorState::Closing)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_WaitBeforeOpen, this, &ABBDoorBase::CheckBeforeOpening, CloseTime, false);

	}


}

void ABBDoorBase::CheckBeforeClosing()
{
	if (DoorState == EDoorState::Opened)
	{
		DoorState = EDoorState::Closing;
		CloseDoor();
	}
	else if (DoorState == EDoorState::Opening)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_WaitBeforeClose, this, &ABBDoorBase::CheckBeforeClosing, OpenTime, false);
	}
}
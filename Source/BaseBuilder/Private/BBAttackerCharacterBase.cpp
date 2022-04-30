// Fill out your copyright notice in the Description page of Project Settings.


#include "BBAttackerCharacterBase.h"


ABBAttackerCharacterBase::ABBAttackerCharacterBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ABBAttackerCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

void ABBAttackerCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void ABBAttackerCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	//PlayerInputComponent->BindAction("Interact", EInputEvent::IE_Pressed, this, &ABBCharacter::Pickup);
	//PlayerInputComponent->BindAction("Interact", EInputEvent::IE_Released, this, &ABBCharacter::Drop);
	
}

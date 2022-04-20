// Fill out your copyright notice in the Description page of Project Settings.


#include "BBCharacter.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ABBCharacter::ABBCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABBCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABBCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABBCharacter::MoveForward(float value)
{
	AddMovementInput(GetActorForwardVector() * value);
}

void ABBCharacter::MoveRight(float value)
{
	AddMovementInput(GetActorRightVector() * value);
}

void ABBCharacter::BeginCrouch()
{
	Crouch();
}

void ABBCharacter::EndCrouch()
{
	UnCrouch();
}

void ABBCharacter::CJump()
{
}


// Called to bind functionality to input
void ABBCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ABBCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABBCharacter::MoveRight);

	PlayerInputComponent->BindAxis("LookUp", this, &ABBCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &ABBCharacter::AddControllerYawInput);
	
	PlayerInputComponent->BindAction("Crouch", EInputEvent::IE_Pressed, this, &ABBCharacter::BeginCrouch);
	PlayerInputComponent->BindAction("Crouch", EInputEvent::IE_Released, this, &ABBCharacter::EndCrouch);

}

void ABBCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABBCharacter, test);
}


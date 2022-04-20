// Fill out your copyright notice in the Description page of Project Settings.


#include "BBCharacter.h"
#include "BBBaseBlock.h"
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

void ABBCharacter::Pickup()
{
	FHitResult HitResult;
	FVector EyeLocation;
	FRotator EyeRotation;
	GetActorEyesViewPoint(EyeLocation,EyeRotation);

	FVector direction = EyeRotation.Vector();
	FVector traceEnd = EyeLocation + (direction * BlocPickupDistance);


	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.bTraceComplex = true;
	
	if(GetWorld()->LineTraceSingleByChannel(HitResult, EyeLocation, traceEnd, ECollisionChannel::ECC_Visibility,QueryParams))
	{
		DrawDebugLine(GetWorld(), EyeLocation, HitResult.ImpactPoint, FColor::Green,false, 1.0f,0,1.0f);

		ABBBaseBlock* BaseBlock = Cast<ABBBaseBlock>(HitResult.GetActor());
		if(BaseBlock)
		{
			BaseBlock->BlockIsActive = true;
			BaseBlock->OwnerCharacter = this;

			CurrentBaseBlock = BaseBlock;
			
			FVector blockTeleportPosition = EyeLocation + (direction * BaseBlock->defaultDistanceBetween);
			BaseBlock->SetActorLocation(blockTeleportPosition);

			if(!HasAuthority())
			{
				ServerPickup();
			}
		}
	}
}

void ABBCharacter::ServerPickup_Implementation()
{
	Pickup();
}

bool ABBCharacter::ServerPickup_Validate()
{
	return true;
}

void ABBCharacter::Drop()
{
	if(CurrentBaseBlock)
	{
		CurrentBaseBlock->BlockIsActive = false;
		CurrentBaseBlock->OwnerCharacter = nullptr;
		CurrentBaseBlock = nullptr;
		
		if(!HasAuthority())
		{
			ServerDrop();
		}
	}
}

void ABBCharacter::ServerDrop_Implementation()
{
	Drop();
}

bool ABBCharacter::ServerDrop_Validate()
{
	return true;
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

	PlayerInputComponent->BindAction("Interact", EInputEvent::IE_Pressed, this, &ABBCharacter::Pickup);
	PlayerInputComponent->BindAction("Interact", EInputEvent::IE_Released, this, &ABBCharacter::Drop);

}

void ABBCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABBCharacter, test);
}


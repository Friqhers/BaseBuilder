// Fill out your copyright notice in the Description page of Project Settings.


#include "BBCharacter.h"
#include "BBBaseBlock.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework\CharacterMovementComponent.h"

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
	if (IsInitiatedJump() != true)
	{
		Crouch();
	}
	else
	{
		Crouch();
		float halfHeight = GetCharacterMovement()->CrouchedHalfHeight;
		AddActorWorldOffset(FVector(0,0,halfHeight/2));
	}
	
}

void ABBCharacter::EndCrouch()
{
	// if (IsInitiatedJump() != true)
	// {
	// 	UnCrouch();
	// }
	UnCrouch();
}

void ABBCharacter::CJump()
{
	//float FirstDelay = FMath::Max(lastJumpTime + TimeBetweenJumps - GetWorld()->TimeSeconds, 0.0f);
	float currentTime = GetWorld()->TimeSeconds;
	if(currentTime-lastJumpTime >= TimeBetweenJumps)
	{
		SetIsJumping(true);
	}
	
}

void ABBCharacter::Pickup()
{
	if(HasAuthority() == false)
	{
		ServerPickup();
	}
	
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
			BaseBlock->SetActorEnableCollision(false);
			BaseBlock->collisionEnabled = false;
			
			BaseBlock->BlockIsActive = true;
			BaseBlock->OwnerCharacter = this;

			CurrentBaseBlock = BaseBlock;
			
			FVector blockTeleportPosition = EyeLocation + (direction * BaseBlock->defaultDistanceBetween);
			BaseBlock->SetActorLocation(blockTeleportPosition);
			
		}
	}
	else
	{
		DrawDebugLine(GetWorld(), EyeLocation, traceEnd, FColor::Red,false, 1.0f,0,1.0f);
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
	if(HasAuthority() == false)
	{
		ServerDrop();
	}
	
	if(CurrentBaseBlock)
	{
		CurrentBaseBlock->SetActorEnableCollision(true);
		CurrentBaseBlock->collisionEnabled = true;
		CurrentBaseBlock->BlockIsActive = false;
		CurrentBaseBlock->OwnerCharacter = nullptr;
		CurrentBaseBlock = nullptr;
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

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ABBCharacter::CJump);

}

bool ABBCharacter::IsInitiatedJump() const
{
	return bIsJumping;
}

void ABBCharacter::SetIsJumping(bool NewJumping)
{
	if(bIsCrouched && NewJumping)
	{
		UnCrouch();
	}
	else if(NewJumping != bIsJumping)
	{
		bIsJumping = NewJumping;
		if(bIsJumping)
		{
			lastJumpTime = GetWorld()->TimeSeconds;
			Jump();
		}
	}

	if(HasAuthority() == false)
	{
		ServerSetIsJumping(NewJumping);
	}
}

void ABBCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

	/* Check if we are no longer falling/jumping */
	if (PrevMovementMode == EMovementMode::MOVE_Falling && GetCharacterMovement()->MovementMode != EMovementMode::MOVE_Falling)
	{
		SetIsJumping(false);
		jumpFinished = true;
	}
}


void ABBCharacter::ServerSetIsJumping_Implementation(bool NewJumping)
{
	SetIsJumping(NewJumping);
}

bool ABBCharacter::ServerSetIsJumping_Validate(bool NewJumping)
{
	return true;
}

void ABBCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABBCharacter, bIsJumping);
}


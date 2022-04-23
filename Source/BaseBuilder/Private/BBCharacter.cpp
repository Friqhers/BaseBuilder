// Fill out your copyright notice in the Description page of Project Settings.


#include "BBCharacter.h"
#include "BBBaseBlock.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework\CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"

// Sets default values
ABBCharacter::ABBCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(RootComponent);

	crouchHalfHeight = GetCharacterMovement()->GetCrouchedHalfHeight();
	capsuleHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

}

// Called when the game starts or when spawned
void ABBCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABBCharacter::Pull()
{
	if(CurrentBaseBlock)
	{
		if(distanceBetween-pullPushPower > 100)
		{
			distanceBetween-= pullPushPower;
		}

		if(!HasAuthority())
		{
			ServerPull();
		}
	}
}

void ABBCharacter::Push()
{
	if(CurrentBaseBlock)
	{
		if(distanceBetween+pullPushPower < 10000)
		{
			distanceBetween+= pullPushPower;
		}

		if(!HasAuthority())
		{
			ServerPush();
		}

	}	
}

void ABBCharacter::ServerPush_Implementation()
{
	Push();
}

bool ABBCharacter::ServerPush_Validate()
{
	return true;
}

void ABBCharacter::ServerPull_Implementation()
{
	Pull();
}

bool ABBCharacter::ServerPull_Validate()
{
	return true;
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
		//CameraComp->AddRelativeLocation(FVector(0,0,halfHeight));

		float cameraHeight = CameraComp->GetRelativeLocation().Z;
		float zOffset = (cameraHeight) - crouchHalfHeight;
		AddActorWorldOffset(FVector(0, 0, zOffset));
		SetIsJumping(false);
		if(!HasAuthority())
		{
			ServerCrouchJump();
		}
		
		
		//GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		
		// GetCapsuleComponent()->SetCapsuleSize(25, 40);
		// GetCapsuleComponent()->AddRelativeLocation(FVector(0,0,48));
		// //AddActorWorldOffset(FVector(0,0,48));
		// bIsCrouchJumped = true;
		
	}
	
}

void ABBCharacter::EndCrouch()
{
	UnCrouch();
}

void ABBCharacter::ServerCrouchJump_Implementation()
{
	float cameraHeight = CameraComp->GetRelativeLocation().Z;
	float zOffset = (cameraHeight) - crouchHalfHeight;
	AddActorWorldOffset(FVector(0,0, zOffset));
}

bool ABBCharacter::ServerCrouchJump_Validate()
{
	return true;
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
			if(BaseBlock->BlockIsActive || BaseBlock->OwnerCharacter)
			{
				return;
			}
			
			BaseBlock->SetActorEnableCollision(false);
			BaseBlock->collisionEnabled = false;
			
			BaseBlock->BlockIsActive = true;
			BaseBlock->OwnerCharacter = this;

			CurrentBaseBlock = BaseBlock;

			//distanceBetween = FVector::Distance(EyeLocation, HitResult.GetActor()->GetActorLocation());
			distanceBetween = FVector::Distance(EyeLocation, HitResult.ImpactPoint);
			blockOffset = HitResult.ImpactPoint - HitResult.GetActor()->GetActorLocation();
			
			//FVector blockTeleportPosition = EyeLocation + (direction * BaseBlock->defaultDistanceBetween);
			//BaseBlock->SetActorLocation(blockTeleportPosition);
			
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

bool ABBCharacter::IsInitiatedJump() const
{
	return bIsJumping;
}

void ABBCharacter::SetIsJumping(bool NewJumping)
{
	if(bIsCrouched && NewJumping)
	{
		if(!GetCharacterMovement()->IsFalling())
		{
			LaunchCharacter(FVector(0,0,420), true, true);
		}
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

void ABBCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	//float cameraHeight = CameraComp->GetRelativeLocation().Z;
	CameraComp->AddRelativeLocation(FVector(0,0, -(capsuleHalfHeight-crouchHalfHeight)));
}

void ABBCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	//float cameraHeight = CameraComp->GetRelativeLocation().Z;
	CameraComp->AddRelativeLocation(FVector(0,0, capsuleHalfHeight-crouchHalfHeight));
}


void ABBCharacter::ServerSetIsJumping_Implementation(bool NewJumping)
{
	SetIsJumping(NewJumping);
}

bool ABBCharacter::ServerSetIsJumping_Validate(bool NewJumping)
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

	PlayerInputComponent->BindAction("Pull", IE_Pressed, this, &ABBCharacter::Pull);
	PlayerInputComponent->BindAction("Push", IE_Pressed, this, &ABBCharacter::Push);

}


void ABBCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABBCharacter, bIsJumping);
	DOREPLIFETIME(ABBCharacter, distanceBetween);
	DOREPLIFETIME(ABBCharacter, blockOffset);
}


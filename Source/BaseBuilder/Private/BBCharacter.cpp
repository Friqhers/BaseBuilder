// Fill out your copyright notice in the Description page of Project Settings.


#include "BBCharacter.h"
#include "BBBaseBlock.h"
#include "BBGameMode.h"
#include "BaseBuilder/BaseBuilder.h"
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

	HealthComponent = CreateDefaultSubobject<UBBHealthComponent>(TEXT("HealthComp"));
	
	crouchHalfHeight = GetCharacterMovement()->GetCrouchedHalfHeight();
	capsuleHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

}

// Called when the game starts or when spawned
void ABBCharacter::BeginPlay()
{
	Super::BeginPlay();

	
	
	if(HasAuthority() )
	{

		HealthComponent->OnHealthChanged.AddDynamic(this, &ABBCharacter::OnHealthChanged);
		
	}
}




// void ABBCharacter::OnRep_BBCharacterType()
// {
// 	if(!DynamicMaterial)
// 	{
// 		UE_LOG(LogTemp, Error, TEXT("DynamicMaterial is null for: %s"), *GetName());
// 		return;
// 	}
// 	
// 	if(BBCharacterType == EBBCharacterType::BaseAttacker)
// 	{
// 		DynamicMaterial->SetVectorParameterValue("BodyColor", FLinearColor(1, 0.144553, 0)); 
// 	}
// 	else
// 	{
// 		DynamicMaterial->SetVectorParameterValue("BodyColor", FLinearColor(0.263795, 0, 1));
// 	}
// }

void ABBCharacter::SetBBCharacterType(EBBCharacterType CharacterType)
{
	if(HasAuthority())
	{
		BBCharacterType = CharacterType;
	}
}

// Called every frame
void ABBCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void ABBCharacter::MoveForward(float value)
{
	if(value != 0)
	{
		float zPos = GetActorLocation().Z - 66;
		float xPos = GetActorLocation().X;
		float yPos = GetActorLocation().Y + 25;
		FVector ForwardPoint1, ForwardPoint2;
		ForwardPoint1 = FVector(xPos,yPos, zPos);
		ForwardPoint2 = FVector(xPos,yPos - 50, zPos);
	
		FVector direction = value > 0 ? GetActorForwardVector() : -GetActorForwardVector();
		FVector traceEnd1 = ForwardPoint1 + direction * BaseBlockCheckDist;
		FVector traceEnd2 = ForwardPoint2 + direction * BaseBlockCheckDist;
		FHitResult Hit;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true;
		
		if(GetWorld()->LineTraceSingleByChannel(Hit, ForwardPoint1, traceEnd1, COLLISION_BASEBLOCK, QueryParams))
		{
			DrawDebugLine(GetWorld(), ForwardPoint1, Hit.ImpactPoint, FColor::Red,false, 1.0f,0,1.0f);
			return;
		}
		else
		{
			DrawDebugLine(GetWorld(), ForwardPoint1, traceEnd1, FColor::Purple,false, 1.0f,0,1.0f);
		}

		
		if(GetWorld()->LineTraceSingleByChannel(Hit, ForwardPoint2, traceEnd2, COLLISION_BASEBLOCK, QueryParams))
		{
			DrawDebugLine(GetWorld(), ForwardPoint2, Hit.ImpactPoint, FColor::Red,false, 1.0f,0,1.0f);
			return;
		}
		else
		{
			DrawDebugLine(GetWorld(), ForwardPoint2, traceEnd2, FColor::Purple,false, 1.0f,0,1.0f);
		}

		AddMovementInput(GetActorForwardVector() * value);
	}
}

void ABBCharacter::MoveRight(float value)
{
	if(value != 0)
	{
		float zPos = GetActorLocation().Z - 66;
		float xPos = GetActorLocation().X + 25;
		float yPos = GetActorLocation().Y;
		FVector ForwardPoint1, ForwardPoint2;
		ForwardPoint1 = FVector(xPos,yPos, zPos);
		ForwardPoint2 = FVector(xPos - 50,yPos, zPos);
	
		FVector direction = value > 0 ? GetActorRightVector() : -GetActorRightVector();
		FVector traceEnd1 = ForwardPoint1 + direction * BaseBlockCheckDist;
		FVector traceEnd2 = ForwardPoint2 + direction * BaseBlockCheckDist;
		FHitResult Hit;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true;
		
		if(GetWorld()->LineTraceSingleByChannel(Hit, ForwardPoint1, traceEnd1, COLLISION_BASEBLOCK, QueryParams))
		{
			DrawDebugLine(GetWorld(), ForwardPoint1, Hit.ImpactPoint, FColor::Red,false, 1.0f,0,1.0f);
			return;
		}
		else
		{
			DrawDebugLine(GetWorld(), ForwardPoint1, traceEnd1, FColor::Orange,false, 1.0f,0,1.0f);
		}

		
		if(GetWorld()->LineTraceSingleByChannel(Hit, ForwardPoint2, traceEnd2, COLLISION_BASEBLOCK, QueryParams))
		{
			DrawDebugLine(GetWorld(), ForwardPoint2, Hit.ImpactPoint, FColor::Red,false, 1.0f,0,1.0f);
			return;
		}
		else
		{
			DrawDebugLine(GetWorld(), ForwardPoint2, traceEnd2, FColor::Orange,false, 1.0f,0,1.0f);
		}

		AddMovementInput(GetActorRightVector() * value);
	}
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
		float zOffset = (capsuleHalfHeight) - crouchHalfHeight;
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


void ABBCharacter::OnRep_Dead()
{
	GetMovementComponent()->StopMovementImmediately();
	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	CapsuleComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn , ECollisionResponse::ECR_Ignore);
	CapsuleComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera , ECollisionResponse::ECR_Ignore);
	CapsuleComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_PhysicsBody , ECollisionResponse::ECR_Ignore);
	
	DetachFromControllerPendingDestroy();
	SetLifeSpan(10.0f);
	
}

void ABBCharacter::OnHealthChanged(UBBHealthComponent* InHealthComp, float Health, float HealthDelta,
                                   const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if(Health <= 0.0f && bDied != true)
	{
		bDied = true;
		//implement Game mode on actor killed
		ABBGameMode* BBGameMode = Cast<ABBGameMode>(GetWorld()->GetAuthGameMode());
		if(BBGameMode)
		{
			BBGameMode->OnActorKilled.Broadcast(GetOwner(), DamageCauser, GetController(),InstigatedBy);
		}
		OnRep_Dead();
	}
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
	
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ABBCharacter::CJump);
	
}


void ABBCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABBCharacter, bIsJumping);
	DOREPLIFETIME(ABBCharacter, bDied);
}


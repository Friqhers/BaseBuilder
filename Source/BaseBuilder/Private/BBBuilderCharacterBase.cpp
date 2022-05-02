// Fill out your copyright notice in the Description page of Project Settings.


#include "BBBuilderCharacterBase.h"
#include "BBBaseBlock.h"
#include "BBGameMode.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework\CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"

ABBBuilderCharacterBase::ABBBuilderCharacterBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ABBBuilderCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
	// if(!DynamicMaterial)
	// {
	// 	DynamicMaterial = UMaterialInstanceDynamic::Create(Material, this);
	// 	GetMesh()->SetMaterial(0, DynamicMaterial);
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

	if(HasAuthority())
	{
		if(blockColor == FLinearColor(0,0,0))
		{
			blockColor = FLinearColor::MakeRandomColor();
		}
	}
	
}

void ABBBuilderCharacterBase::Tick(float DeltaTime)
{

	Super::Tick(DeltaTime);
}

void ABBBuilderCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Interact", EInputEvent::IE_Pressed, this, &ABBBuilderCharacterBase::Pickup);
	PlayerInputComponent->BindAction("Interact", EInputEvent::IE_Released, this, &ABBBuilderCharacterBase::Drop);

	PlayerInputComponent->BindAction("Pull", IE_Pressed, this, &ABBBuilderCharacterBase::Pull);
	PlayerInputComponent->BindAction("Push", IE_Pressed, this, &ABBBuilderCharacterBase::Push);

	PlayerInputComponent->BindAction("ToggleBlockLock", IE_Pressed, this, &ABBBuilderCharacterBase::ToggleBlockLock);
}

void ABBBuilderCharacterBase::Pull()
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

void ABBBuilderCharacterBase::Push()
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


void ABBBuilderCharacterBase::ServerPush_Implementation()
{
	Push();
}

bool ABBBuilderCharacterBase::ServerPush_Validate()
{
	return true;
}

void ABBBuilderCharacterBase::ServerPull_Implementation()
{
	Pull();
}

bool ABBBuilderCharacterBase::ServerPull_Validate()
{
	return true;
}

void ABBBuilderCharacterBase::ToggleBlockLock()
{
	if(!bCanLockBlocks)
	{
		return;
	}
	
	if(!HasAuthority())
	{
		ServerToggleBlockLock();
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
		ABBBaseBlock* BaseBlock = Cast<ABBBaseBlock>(HitResult.GetActor());
		if(BaseBlock)
		{
			if(!BaseBlock->BlockIsActive)
			{
				if(BaseBlock->BlockIsLockedToOwner)
				{
					//unlock
					if(BaseBlock->OwnerCharacter == this)
					{
						BaseBlock->Unlock();
					}
				}
				else if(BaseBlock->BlockIsLockedToOwner == false)
				{	
					//lock
					if(BaseBlock->OwnerCharacter == nullptr)
					{
						BaseBlock->Lock(this);
					}
				}
			}
		}
	}
}


void ABBBuilderCharacterBase::ServerToggleBlockLock_Implementation()
{
	ToggleBlockLock();
}

bool ABBBuilderCharacterBase::ServerToggleBlockLock_Validate()
{
	return true;
}

void ABBBuilderCharacterBase::Pickup()
{
	if(!bCanMoveBlocks)
	{
		return;
	}
	
	if(HasAuthority() == false)
	{
		ServerPickup();
	}
	
	FHitResult HitResult;
	FVector EyeLocation;
	FRotator EyeRotation;
	GetActorEyesViewPoint(EyeLocation,EyeRotation);

	EyeLocation = CameraComp->GetComponentLocation();
	
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
			//if block is already in move
			if(BaseBlock->BlockIsActive)
			{
				return;
			}
			
			//if we try to move someone else's block just return;
			if(BaseBlock->BlockIsLockedToOwner && BaseBlock->OwnerCharacter != this)
			{
				return;
			}
			
			BaseBlock->SetActorEnableCollision(false);
			BaseBlock->collisionEnabled = false;
			
			
			BaseBlock->OwnerCharacter = this;

			if(HasAuthority())
			{
				BaseBlock->BlockIsActive = true;
				BaseBlock->OnRep_BlockIsActive();
			}

			
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

void ABBBuilderCharacterBase::ServerPickup_Implementation()
{
	Pickup();
}

bool ABBBuilderCharacterBase::ServerPickup_Validate()
{
	return true;
}

void ABBBuilderCharacterBase::Drop()
{
	if(HasAuthority() == false)
	{
		ServerDrop();
	}
	
	if(CurrentBaseBlock && CurrentBaseBlock->OwnerCharacter == this)
	{
		CurrentBaseBlock->SetActorEnableCollision(true);
		
		if(HasAuthority())
		{
			CurrentBaseBlock->collisionEnabled = true;
			CurrentBaseBlock->BlockIsActive = false;
			CurrentBaseBlock->OnRep_BlockIsActive();

			//if block is not locked, reset the owner
			if(!CurrentBaseBlock->BlockIsLockedToOwner)
			{
				CurrentBaseBlock->OwnerCharacter = nullptr;
			}
			//
			// CurrentBaseBlock->SetActorLocation(CurrentBaseBlock->GetActorLocation());
			// CurrentBaseBlock->UpdateComponentTransforms();
			// CurrentBaseBlock->UpdateAllReplicatedComponents();
			// CurrentBaseBlock->BaseBlockMesh->UpdateCollisionProfile();
			// CurrentBaseBlock->BaseBlockMesh->UpdateCollisionFromStaticMesh();
			// CurrentBaseBlock->ForceNetUpdate();
		}
		
		CurrentBaseBlock = nullptr;		
	}
}

void ABBBuilderCharacterBase::ServerDrop_Implementation()
{
	Drop();
}

bool ABBBuilderCharacterBase::ServerDrop_Validate()
{
	return true;
}

void ABBBuilderCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABBBuilderCharacterBase, distanceBetween);
	DOREPLIFETIME(ABBBuilderCharacterBase, blockOffset);
	DOREPLIFETIME(ABBBuilderCharacterBase, blockColor);
	DOREPLIFETIME(ABBBuilderCharacterBase, bCanMoveBlocks);
	DOREPLIFETIME(ABBBuilderCharacterBase, bCanLockBlocks);
}

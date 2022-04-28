// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BBCharacter.generated.h"


class ABBBaseBlock;
class UCameraComponent;

UENUM(BlueprintType)
enum class EBBCharacterType : uint8
{
	BaseBuilder,
	BaseAttacker
};

// static FLinearColor colors[32] =
// {
// 	FLinearColor::Blue,
// 	FLinearColor::Green,
// 	FLinearColor::Red,
// 	FLinearColor::Yellow,
// 	FLinearColor::
// };


UCLASS()
class BASEBUILDER_API ABBCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABBCharacter();
	
	UPROPERTY(VisibleanyWhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* CameraComp;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void BeginCrouch();
	void EndCrouch();
	void CJump();

	void Pickup();
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerPickup();
public:
	void Drop();
protected:
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerDrop();
	
	/// Pull and Push functions
	void Pull();
	void Push();
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerPull();
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerPush();
	
	//Block lock and unlock functions
	void ToggleBlockLock();
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerToggleBlockLock();

	UPROPERTY()
	ABBBaseBlock* CurrentBaseBlock = nullptr;
public:
	UPROPERTY(EditAnywhere, Category = "Character Options")
	UMaterialInterface* Material;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character Options")
	int BlocPickupDistance = 1000;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character Options", Replicated)
	FLinearColor blockColor = FLinearColor(0,0,0);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character Options")
	int pullPushPower = 10;
	
	UPROPERTY(Replicated, BlueprintReadOnly)
	int distanceBetween;

	UPROPERTY(Replicated, BlueprintReadOnly)
	FVector blockOffset;

	UPROPERTY(BlueprintReadWrite)
	UMaterialInstanceDynamic* DynamicMaterial;
public:
	/**
	 * Character type ....
	 */
	UPROPERTY(Replicated, BlueprintReadOnly)
	bool bCanMoveBlocks = false;
	
	UPROPERTY(Replicated, BlueprintReadOnly)
	bool bCanLockBlocks = false;

	UPROPERTY(Replicated, BlueprintReadOnly)
	EBBCharacterType BBCharacterType;

	void SetBBCharacterType(EBBCharacterType CharacterType);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	bool bIsCrouchJumped = false;
	
	/* Is character currently performing a jump action. Resets on landed.  */
	UPROPERTY(Transient, Replicated)
	bool bIsJumping;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	bool IsInitiatedJump() const;

	void SetIsJumping(bool NewJumping);

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerSetIsJumping(bool NewJumping);

	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0) override;

	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerCrouchJump();

protected:
	FTimerHandle TimerHandle_FinishJump;

	void FinishJump();
	
	bool jumpFinished = false;

	float lastJumpTime = 0;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character Options")
	float TimeBetweenJumps = 0.67f;

	float crouchHalfHeight;
	float capsuleHalfHeight;
};


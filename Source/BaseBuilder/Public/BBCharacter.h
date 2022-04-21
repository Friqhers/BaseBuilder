// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BBCharacter.generated.h"

class ABBBaseBlock;
class UCameraComponent;

UCLASS()
class BASEBUILDER_API ABBCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABBCharacter();

protected:

	UPROPERTY(VisibleanyWhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* CameraComp;
	
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
	
	void Drop();
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerDrop();

	ABBBaseBlock* CurrentBaseBlock = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character Options")
	int BlocPickupDistance = 1000;

	void Pull();
	void Push();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerPull();
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerPush();
public:
	UPROPERTY(BlueprintReadOnly)
	int distanceBetween;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character Options")
	int pullPushPower = 10;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
public:
	/* Is character currently performing a jump action. Resets on landed.  */
	UPROPERTY(Transient, Replicated)
	bool bIsJumping;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	bool IsInitiatedJump() const;

	void SetIsJumping(bool NewJumping);

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerSetIsJumping(bool NewJumping);

	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0) override;
protected:
	FTimerHandle TimerHandle_FinishJump;

	void FinishJump();
	
	bool jumpFinished = false;

	float lastJumpTime = 0;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character Options")
	float TimeBetweenJumps = 0.67f;


};


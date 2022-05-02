// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BBHealthComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "BBCharacter.generated.h"


class ABBBaseBlock;
class UCameraComponent;
class UBBHealthComponent;

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

	// UPROPERTY(VisibleanyWhere, BlueprintReadOnly, Category = "Components")
	// USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleanyWhere, BlueprintReadOnly, Category = "Components")
	UArrowComponent* ForwardArrowComponent1;
	UPROPERTY(VisibleanyWhere, BlueprintReadOnly, Category = "Components")
	UArrowComponent* ForwardArrowComponent2;

	UPROPERTY(VisibleanyWhere, BlueprintReadOnly, Category = "Components")
	UArrowComponent* RightArrowComponent1;
	UPROPERTY(VisibleanyWhere, BlueprintReadOnly, Category = "Components")
	UArrowComponent* RightArrowComponent2;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void BeginCrouch();
	void EndCrouch();
	void CJump();

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character Options")
	float TimeBetweenJumps = 0.67f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character Options")
	float BaseBlockCheckDist = 10.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character Options")
	float SlowMultiplier = 10.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character Options")
	float CrouchJumpZSpeed = 370.0f;	
	
	

protected:
	FTimerHandle TimerHandle_FinishJump;
	
	bool jumpFinished = false;

	float lastJumpTime = 0;
	
	float crouchHalfHeight;
	float capsuleHalfHeight;

public:
	//Health, armor related
	UPROPERTY(ReplicatedUsing = OnRep_Dead, BlueprintReadOnly, Category = "Player")
	bool bDied;
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBBHealthComponent* HealthComponent;

	UFUNCTION()
	void OnRep_Dead();

	UFUNCTION()
	void OnHealthChanged(UBBHealthComponent* InHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	
};


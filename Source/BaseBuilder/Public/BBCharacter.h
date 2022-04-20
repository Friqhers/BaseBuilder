// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BBCharacter.generated.h"

class ABBBaseBlock;

UCLASS()
class BASEBUILDER_API ABBCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABBCharacter();

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
	
	void Drop();
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerDrop();

	ABBBaseBlock* CurrentBaseBlock = nullptr;
	
	UPROPERTY(BlueprintReadWrite, Replicated)
	bool test;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character Options")
	int BlocPickupDistance = 1000;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
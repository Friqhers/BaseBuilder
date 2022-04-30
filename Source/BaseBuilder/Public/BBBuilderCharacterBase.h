// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BBCharacter.h"
#include "BBBuilderCharacterBase.generated.h"

class ABBBaseBlock;
class UCameraComponent;
/**
 * 
 */
UCLASS()
class BASEBUILDER_API ABBBuilderCharacterBase : public ABBCharacter
{
	GENERATED_BODY()
public:
	ABBBuilderCharacterBase();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
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
	UPROPERTY(Replicated, BlueprintReadWrite)
	bool bCanMoveBlocks = false;
	
	UPROPERTY(Replicated, BlueprintReadWrite)
	bool bCanLockBlocks = false;
	



};

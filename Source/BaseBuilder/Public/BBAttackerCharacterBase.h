// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BBCharacter.h"
#include "BBAttackerCharacterBase.generated.h"

/**
 * 
 */
UCLASS()
class BASEBUILDER_API ABBAttackerCharacterBase : public ABBCharacter
{
	GENERATED_BODY()
public:
	ABBAttackerCharacterBase();
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	
};

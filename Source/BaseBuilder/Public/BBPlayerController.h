// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BBPlayerController.generated.h"

class ABBCharacter;
enum class EBBCharacterType : uint8;
/**
 * 
 */
UCLASS()
class BASEBUILDER_API ABBPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Character Options")
	float RespawnTime = 10.0f;
	FTimerDelegate TimerHandle_RespawnDelegate;
	FTimerHandle TimerHandle_RespawnTimer;

	void ResetRespawnTimer();
	
	UFUNCTION()
	void RespawnTimer(FVector spawnPos, FRotator spawnRot, TSubclassOf<ABBCharacter> characterClass);

	UFUNCTION()
	void RespawnCharacter(FVector spawnPos, FRotator spawnRot, TSubclassOf<ABBCharacter> characterClass);
	
	UPROPERTY(Replicated, BlueprintReadOnly)
	EBBCharacterType BBCharacterType;

	void SetBBCharacterType(EBBCharacterType CharacterType);
	
};

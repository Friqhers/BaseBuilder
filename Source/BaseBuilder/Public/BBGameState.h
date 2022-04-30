// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BBCharacter.h"
#include "GameFramework/GameState.h"
#include "BBGameState.generated.h"

UENUM(BlueprintType)
enum class EBBGameState : uint8
{
	NULLState,
	WaitingForPlayers,  // current player count is zero or one, there must be at least 2 players to start the game
	StartingNewRound,
	BaseBuildingTime,   // base builders building their bases, zombies are waiting
	ClimbBaseCountDown, // base builders are climbing to their bases, zombies are waiting
	BaseDefending,      // zombie's door opens and they are attacking to bases
	BaseBuilderVictory, // base defending time is finished and at least one base builder is alive
	BaseAttackersVictory// base defending time is finished and no base builder is alive
};

/**
 * 
 */
UCLASS()
class BASEBUILDER_API ABBGameState : public AGameState //can run on clients and server
{
	GENERATED_BODY()

public:
	void SetGameState(EBBGameState NewState);

	EBBGameState GetGameState();

	void SetRemainingSecondsForCurrentState(float newSeconds);

	
protected:
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "GameState")
	float RemainingSecondsForCurrentState = 0;
	
	UPROPERTY(ReplicatedUsing = OnRep_BBGameState, BlueprintReadOnly, Category = "GameState")
	EBBGameState BBGameState;

	UFUNCTION()
	void OnRep_BBGameState(EBBGameState OldState);

	UFUNCTION(BlueprintImplementableEvent, Category = "GameState")
	void BBGameStateChanged(EBBGameState NewState, EBBGameState OldState);
	
};

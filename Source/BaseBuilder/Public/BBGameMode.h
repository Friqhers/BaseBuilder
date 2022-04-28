// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BBGameState.h"
#include "GameFramework/GameMode.h"
#include "BBGameMode.generated.h"

/**

	WaitingForPlayers, // current player count is zero or one, there must be at least 2 players to start the game
	
	BaseBuildingTime, //base builders building their bases, zombies are waiting
	
	ClimbBaseCountDown, //base builders are climbing to their bases, zombies are waiting

	BaseDefending, // zombie's door opens and they are attacking to bases
	
	BaseBuilderVictory // base defending time is finished and at least one base builder is alive
	
	ZombieVictory // base defending time is finished and no base builder is alive

 */

class APlayerStart;
class ABBDoorBase;
class ABBCharacter;
class ABBPlayerState;

UCLASS()
class BASEBUILDER_API ABBGameMode : public AGameMode //can run on only server
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn")
	TSubclassOf<APlayerStart> baseBuilderSpawnPositionClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn")
	TSubclassOf<APlayerStart> baseAttackerSpawnPositionClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn")
	TSubclassOf<ABBCharacter> CharacterClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn")
	TSubclassOf<ABBDoorBase> baseAttackerDoorClass;

	TArray<ABBDoorBase*> baseAttackerDoors;
	TArray<APlayerStart*> baseBuilderSpawnPositions;
	TArray<APlayerStart*> baseAttackerSpawnPositions;
	
	ABBGameMode();
	
	//called when the game mode is created
	virtual void StartPlay() override;
	
	virtual void Tick(float DeltaSeconds) override;

	virtual void OnPostLogin(AController* NewPlayer) override;

	virtual void Logout(AController* Exiting) override;

	//virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;

	void SetBBGameState(EBBGameState NewState);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	EBBGameState GetBBGameState();

	int GetCurrentPlayerCount();

	bool GetAnyBaseBuilderAlive();

	void StartNewRound();
	void ChangeToBaseClimbingState();
	void ChangeToBaseDefendingState();
	void DetermineVictory();
	void ChangeToBaseBuildersVictory();
	void ChaneToBaseAttackersVictory();
	void RestartRound();
	void SwapTeams();

	int waitedTime;
	void UpdateRemainingSecondsTimer();
	FTimerHandle TimerHandle_UpdateRemainingSecondsTimer;
protected:
	ABBGameState* BBGameState;
	TArray<APlayerController*> baseBuilders;
	TArray<APlayerController*> baseAttackers;
	
	
	UPROPERTY(EditDefaultsOnly, Category = "Game Options")
	float StartRoundWaitTime = 10.0f;
	FTimerHandle TimerHandle_StartingRound;

	UPROPERTY(EditDefaultsOnly, Category = "Game Options")
	float BaseBuildingTime = 60.0f;
	FTimerHandle TimerHandle_BaseBuildingTime;

	UPROPERTY(EditDefaultsOnly, Category = "Game Options")
	float ClimbBaseTime = 20.0f;
	FTimerHandle TimerHandle_ClimbBaseCountDown;

	UPROPERTY(EditDefaultsOnly, Category = "Game Options")
	float BaseDefendTime = 40.0f;
	FTimerHandle TimerHandle_BaseDefendingTime;
	
	
};


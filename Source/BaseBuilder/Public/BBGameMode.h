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
class ABBAttackerCharacterBase;
class ABBBuilderCharacterBase;
class ABBPlayerController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnActorKilled, AActor*, KilledActor, AActor*, KillerActor, AController*, KilledController, AController*, KillerController);

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
	TSubclassOf<ABBCharacter> BaseBuilderCharacterClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn")
	TSubclassOf<ABBCharacter> BaseAttackerCharacterClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn")
	TSubclassOf<ABBDoorBase> baseAttackerDoorClass;

	TArray<ABBDoorBase*> baseAttackerDoors;
	TArray<APlayerStart*> baseBuilderSpawnPositions;
	TArray<APlayerStart*> baseAttackerSpawnPositions;

	UPROPERTY(BlueprintAssignable)
	FOnActorKilled OnActorKilled;

	UFUNCTION()
	void OnActorKilledEvent(AActor* KilledActor, AActor* KillerActor, AController* KilledController, AController* KillerController);
	
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
	void FindSpawnPositions();
	/***************/
	//Game flow logic funcs.
	void StartNewRound();
	void ChangeToBaseClimbingState();
	void ChangeToBaseDefendingState();
	void DetermineVictory();
	void ChangeToBaseBuildersVictory();
	void ChaneToBaseAttackersVictory();
	void RestartRound();
	void SwapTeams();
	/***************/

	UFUNCTION()
	void RespawnDeadPlayer(AController* ActorToRespawn);
	
	
	//sets default base builder char. options
	void SetBaseBuilderCharacterOptions(ABBBuilderCharacterBase* targetCharacter);
	//sets default base attacker char. options
	void SetBaseAttackerCharacterOptions(ABBAttackerCharacterBase* targetCharacter);

	int waitedTime;
	void UpdateRemainingSecondsTimer();
	FTimerHandle TimerHandle_UpdateRemainingSecondsTimer;
protected:
	ABBGameState* BBGameState;
	TArray<ABBPlayerController*> baseBuilders;
	TArray<ABBPlayerController*> baseAttackers;
	TArray<ABBPlayerController*> respawnedBaseBuilders; // they are now in base attacker state
	
	
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


// Fill out your copyright notice in the Description page of Project Settings.


#include "BBGameMode.h"

#include "BBCharacter.h"
#include "BBGameState.h"
#include "BBPlayerState.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Components/ArrowComponent.h"
#include "BBDoorBase.h"


ABBGameMode::ABBGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.0f;

	GameStateClass = ABBGameState::StaticClass();
	PlayerStateClass = ABBPlayerState::StaticClass();

	BBGameState = GetGameState<ABBGameState>();
}


void ABBGameMode::StartPlay()
{
	Super::StartPlay();
	
	SetBBGameState(EBBGameState::WaitingForPlayers);

	OnActorKilled.AddDynamic(this, &ABBGameMode::OnActorKilledEvent);
	
}

void ABBGameMode::OnActorKilledEvent(AActor* KilledActor, AActor* KillerActor, AController* KilledController, AController* KillerController)
{
	TimerHandle_RespawnDelegate.BindUFunction(this, FName("RespawnDeadPlayer"), KilledController);
	GetWorldTimerManager().SetTimer(TimerHandle_RespawnTimer, TimerHandle_RespawnDelegate,RespawnTime, false);
}


void ABBGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(BBGameState->GetGameState() == EBBGameState::WaitingForPlayers)
	{
		if(NumPlayers > 1)
		{
			SetBBGameState(EBBGameState::StartingNewRound);
			GetWorldTimerManager().ClearTimer(TimerHandle_StartingRound);
			GetWorldTimerManager().SetTimer(TimerHandle_StartingRound, this, &ABBGameMode::StartNewRound, StartRoundWaitTime, false);

			waitedTime = StartRoundWaitTime;
			GetWorldTimerManager().SetTimer(TimerHandle_UpdateRemainingSecondsTimer, this, &ABBGameMode::UpdateRemainingSecondsTimer, 1.0f, true);
			//StartNewRound();
		}
	}
	else if(BBGameState->GetGameState() == EBBGameState::BaseDefending ||
		BBGameState->GetGameState() == EBBGameState::BaseBuildingTime ||
		BBGameState->GetGameState() == EBBGameState::ClimbBaseCountDown)
	{
		if(GetAnyBaseBuilderAlive() == false)
		{
			//base attacker win
			GetWorldTimerManager().ClearTimer(TimerHandle_RespawnTimer);
			ChaneToBaseAttackersVictory();
		}
	}
}

void ABBGameMode::UpdateRemainingSecondsTimer()
{
	waitedTime = waitedTime -1;
	BBGameState->SetRemainingSecondsForCurrentState(waitedTime);
	if(waitedTime == 0)
	{
		GetWorldTimerManager().ClearTimer(TimerHandle_UpdateRemainingSecondsTimer);
	}
}


//called after new player joins
void ABBGameMode::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);
	int numberOfBuilders = baseBuilders.Num();
	int numberOfAttackers = baseAttackers.Num();

	if(numberOfAttackers == numberOfBuilders)
	{
		baseBuilders.Add(Cast<APlayerController>(NewPlayer));
	}
	else if(numberOfAttackers > numberOfBuilders)
	{
		baseBuilders.Add(Cast<APlayerController>(NewPlayer));
	}
	else //numberOfBuilders > numberOfAttackers
	{
		baseAttackers.Add(Cast<APlayerController>(NewPlayer));
	}

	if(BBGameState == nullptr)
	{
		BBGameState = GetGameState<ABBGameState>();
	}
	
	// if(BBGameState->GetGameState() == EBBGameState::BaseBuildingTime || BBGameState->GetGameState() == EBBGameState::BaseDefending || BBGameState->GetGameState() == EBBGameState::ClimbBaseCountDown)
	// {
	// 	FActorSpawnParameters SpawnParameters;
	// 	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	//
	// 	FVector spawnPos = baseAttackerSpawnPositions[0]->GetActorLocation();
	// 	FRotator spawnRot = baseAttackerSpawnPositions[0]->GetActorRotation();
	// 	
	// 	ABBCharacter* CharacterToPosses = GetWorld()->SpawnActor<ABBCharacter>(CharacterClass, spawnPos, spawnRot, SpawnParameters);
	// 	if(CharacterToPosses)
	// 	{
	// 		NewPlayer->Possess(CharacterToPosses);
	// 	}
	// }
	
}

void ABBGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	
	int i = baseBuilders.Find(Cast<APlayerController>(Exiting));
	if(i != INDEX_NONE)
	{
		baseBuilders.RemoveAt(i);
		return;
	}
	else
	{
		i = baseAttackers.Find(Cast<APlayerController>(Exiting));
		if(i != INDEX_NONE)
		{
			baseAttackers.RemoveAt(i);
			return;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Değişik bişi oldu, çıkan oyuncuyu bulup silemedik gardaş"));
}


void ABBGameMode::StartNewRound()
{
	//Reset the level (like base blocks etc.)
	ResetLevel();
	
	
	//find basebuilder spawn positions if it is not found yet
	if(baseBuilderSpawnPositions.Num() <=0)
	{
		TArray<AActor*> baseBuilderSpawnActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), baseBuilderSpawnPositionClass, baseBuilderSpawnActors);
		for (AActor* BaseBuilderSpawnActor : baseBuilderSpawnActors)
		{
			baseBuilderSpawnPositions.Add(Cast<APlayerStart>(BaseBuilderSpawnActor));
		}
	}

	//find baseattackers spawn positions if it is not found yet
	if(baseAttackerSpawnPositions.Num() <= 0)
	{
		TArray<AActor*> baseAttackerSpawnActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), baseAttackerSpawnPositionClass, baseAttackerSpawnActors);
		for (AActor* baseAttackerSpawnActor : baseAttackerSpawnActors)
		{
			baseAttackerSpawnPositions.Add(Cast<APlayerStart>(baseAttackerSpawnActor));
		}
	}

	if(baseAttackerDoors.Num() <= 0)
	{
		TArray<AActor*> baseAttackerDoorActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), baseAttackerDoorClass, baseAttackerDoorActors);
		for(AActor* baseAttackerDoorActor : baseAttackerDoorActors)
		{
			baseAttackerDoors.Add(Cast<ABBDoorBase>(baseAttackerDoorActor));
		}

	}

	
	
	//Spawn all joined base builders
	for (APlayerController* baseBuilderController : baseBuilders)
	{
		int randomSpawnPosIndex = FMath::RandRange(0, baseBuilderSpawnPositions.Num()-1);
		FVector spawnPos = baseBuilderSpawnPositions[randomSpawnPosIndex]->GetActorLocation();
		FRotator spawnRot = baseBuilderSpawnPositions[randomSpawnPosIndex]->GetActorRotation();

		//if controller has character, just teleport
		ABBCharacter* baseBuilderCharacter = Cast<ABBCharacter>(baseBuilderController->GetCharacter());
		if(baseBuilderCharacter)
		{
			baseBuilderCharacter->TeleportTo(spawnPos, spawnRot);
			SetBaseBuilderCharacterOptions(baseBuilderCharacter);
			continue;
		}

		
		//if controller has no character, spawn a character and posses 
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		
		ABBCharacter* CharacterToPosses = GetWorld()->SpawnActor<ABBCharacter>(CharacterClass, spawnPos, spawnRot, SpawnParameters);
		if(CharacterToPosses)
		{
			SetBaseBuilderCharacterOptions(CharacterToPosses);
			baseBuilderController->Possess(CharacterToPosses);
			CharacterToPosses->SetActorRotation(spawnRot);
		}
	}
	
	
	//Spawn all joined base attackers
	for (APlayerController* baseAttackerController : baseAttackers)
	{
		int randomSpawnPosIndex = FMath::RandRange(0, baseAttackerSpawnPositions.Num()-1);
		FVector spawnPos = baseAttackerSpawnPositions[randomSpawnPosIndex]->GetActorLocation();
		FRotator spawnRot = baseAttackerSpawnPositions[randomSpawnPosIndex]->GetActorRotation();

		//if controller has character, just teleport
		ABBCharacter* baseAttackerCharacter = Cast<ABBCharacter>(baseAttackerController->GetCharacter());
		if(baseAttackerCharacter)
		{
			baseAttackerCharacter->TeleportTo(spawnPos, spawnRot);
			SetBaseAttackerCharacterOptions(baseAttackerCharacter);
			continue;
		}
		
		//if controller has no character, spawn a character and posses 
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		
		ABBCharacter* CharacterToPosses = GetWorld()->SpawnActor<ABBCharacter>(CharacterClass, spawnPos, spawnRot, SpawnParameters);
		if(CharacterToPosses)
		{
			SetBaseAttackerCharacterOptions(CharacterToPosses);
			baseAttackerController->Possess(CharacterToPosses);
			CharacterToPosses->SetActorRotation(spawnRot);

		}
	}


	
	BBGameState->SetGameState(EBBGameState::BaseBuildingTime);
	GetWorldTimerManager().ClearTimer(TimerHandle_BaseBuildingTime);
	GetWorldTimerManager().SetTimer(TimerHandle_BaseBuildingTime, this, &ABBGameMode::ChangeToBaseClimbingState, BaseBuildingTime, false);

	waitedTime = BaseBuildingTime;
	GetWorldTimerManager().SetTimer(TimerHandle_UpdateRemainingSecondsTimer, this, &ABBGameMode::UpdateRemainingSecondsTimer, 1.0f, true);
	
}


void ABBGameMode::ChangeToBaseClimbingState()
{
	BBGameState->SetGameState(EBBGameState::ClimbBaseCountDown);

	//set base builders' positions to starting positions
	for (APlayerController* baseBuilderController : baseBuilders)
	{
		ABBCharacter* baseBuilderCharacter = Cast<ABBCharacter>(baseBuilderController->GetCharacter());
		if(baseBuilderCharacter)
		{
			baseBuilderCharacter->bCanMoveBlocks = false;
			baseBuilderCharacter->bCanLockBlocks = false;
			
			int randomSpawnPosIndex = FMath::RandRange(0, baseBuilderSpawnPositions.Num()-1);
		
			FVector spawnPos = baseBuilderSpawnPositions[randomSpawnPosIndex]->GetActorLocation();
			FRotator spawnRot = baseBuilderSpawnPositions[randomSpawnPosIndex]->GetActorForwardVector().Rotation();

			baseBuilderCharacter->TeleportTo(spawnPos, spawnRot);
		}
	}

	GetWorldTimerManager().SetTimer(TimerHandle_ClimbBaseCountDown, this, &ABBGameMode::ChangeToBaseDefendingState, ClimbBaseTime, false);
	
	waitedTime = ClimbBaseTime;
	GetWorldTimerManager().SetTimer(TimerHandle_UpdateRemainingSecondsTimer, this, &ABBGameMode::UpdateRemainingSecondsTimer, 1.0f, true);
}

void ABBGameMode::ChangeToBaseDefendingState()
{
	BBGameState->SetGameState(EBBGameState::BaseDefending);

	//open the baseattackers door
	for(ABBDoorBase* attackerDoor : baseAttackerDoors)
	{
		if(attackerDoor)
		{
			attackerDoor->CheckBeforeOpening();
		}
	}
	//.....

	GetWorldTimerManager().SetTimer(TimerHandle_BaseDefendingTime, this, &ABBGameMode::DetermineVictory, BaseDefendTime, false);

	waitedTime = BaseDefendTime;
	GetWorldTimerManager().SetTimer(TimerHandle_UpdateRemainingSecondsTimer, this, &ABBGameMode::UpdateRemainingSecondsTimer, 1.0f, true);
}

void ABBGameMode::DetermineVictory()
{
	if(GetAnyBaseBuilderAlive())
	{
		ChangeToBaseBuildersVictory();
	}
	else
	{
		ChaneToBaseAttackersVictory();
	}
}

void ABBGameMode::ChangeToBaseBuildersVictory()
{
	BBGameState->SetGameState(EBBGameState::BaseBuilderVictory);

	
	GetWorldTimerManager().SetTimer(TimerHandle_BaseDefendingTime, this, &ABBGameMode::RestartRound, 5.0f, false);

	waitedTime = 5.0f;
	GetWorldTimerManager().SetTimer(TimerHandle_UpdateRemainingSecondsTimer, this, &ABBGameMode::UpdateRemainingSecondsTimer, 1.0f, true);
}

void ABBGameMode::ChaneToBaseAttackersVictory()
{
	BBGameState->SetGameState(EBBGameState::BaseAttackersVictory);

	GetWorldTimerManager().SetTimer(TimerHandle_BaseDefendingTime, this, &ABBGameMode::RestartRound, 5.0f, false);

	waitedTime = 5.0f;
	GetWorldTimerManager().SetTimer(TimerHandle_UpdateRemainingSecondsTimer, this, &ABBGameMode::UpdateRemainingSecondsTimer, 1.0f, true);

}

void ABBGameMode::RestartRound()
{
	SwapTeams();

	StartNewRound();
}

void ABBGameMode::SwapTeams()
{
	TArray<APlayerController*> tempBuilders = baseBuilders;
	baseBuilders = baseAttackers;
	baseAttackers = tempBuilders;
	
	for (APlayerController* respawnedBaseBuilder : respawnedBaseBuilders)
	{
		baseAttackers.Add(respawnedBaseBuilder);
	}
	respawnedBaseBuilders.Empty();
}

void ABBGameMode::RespawnDeadPlayer(AController* ActorToRespawn)
{
	
	APlayerController* playerController = Cast<APlayerController>(ActorToRespawn);
	int index = baseAttackers.Find(playerController);
	if(index != INDEX_NONE)
	{
		int randomSpawnPosIndex = FMath::RandRange(0, baseAttackerSpawnPositions.Num()-1);
		FVector spawnPos = baseAttackerSpawnPositions[randomSpawnPosIndex]->GetActorLocation();
		FRotator spawnRot = baseAttackerSpawnPositions[randomSpawnPosIndex]->GetActorRotation();
		
		
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		
		ABBCharacter* CharacterToPosses = GetWorld()->SpawnActor<ABBCharacter>(CharacterClass, spawnPos, spawnRot, SpawnParameters);
		if(CharacterToPosses)
		{
			SetBaseAttackerCharacterOptions(CharacterToPosses);
			baseAttackers[index]->Possess(CharacterToPosses);
			CharacterToPosses->SetActorRotation(spawnRot);

		}
	}
	else
	{
		//if base builder respawns, he will be turned to base attacker
		index = baseBuilders.Find(playerController);
		if(index != INDEX_NONE)
		{
			int randomSpawnPosIndex = FMath::RandRange(0, baseAttackerSpawnPositions.Num()-1);
			FVector spawnPos = baseAttackerSpawnPositions[randomSpawnPosIndex]->GetActorLocation();
			FRotator spawnRot = baseAttackerSpawnPositions[randomSpawnPosIndex]->GetActorRotation();

			FActorSpawnParameters SpawnParameters;
			SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		
			ABBCharacter* CharacterToPosses = GetWorld()->SpawnActor<ABBCharacter>(CharacterClass, spawnPos, spawnRot, SpawnParameters);
			if(CharacterToPosses)
			{
				SetBaseAttackerCharacterOptions(CharacterToPosses);
				baseBuilders[index]->Possess(CharacterToPosses);
				CharacterToPosses->SetActorRotation(spawnRot);
				
				respawnedBaseBuilders.Add(baseBuilders[index]);
				baseBuilders.RemoveAt(index);
				
			}
		}
	}
}

void ABBGameMode::SetBaseBuilderCharacterOptions(ABBCharacter* targetCharacter)
{
	targetCharacter->BBCharacterType = EBBCharacterType::BaseBuilder;
	targetCharacter->bCanMoveBlocks = true;
	targetCharacter->bCanLockBlocks = true;
	targetCharacter->DynamicMaterial->SetVectorParameterValue("BodyColor", FLinearColor(0.263795, 0, 1));
}

void ABBGameMode::SetBaseAttackerCharacterOptions(ABBCharacter* targetCharacter)
{
	targetCharacter->BBCharacterType = EBBCharacterType::BaseAttacker;
	targetCharacter->bCanMoveBlocks = false;
	targetCharacter->bCanLockBlocks = false;
	targetCharacter->DynamicMaterial->SetVectorParameterValue("BodyColor", FLinearColor(1, 0.144553, 0)); 
}


void ABBGameMode::SetBBGameState(EBBGameState NewState)
{
	if(BBGameState)
	{
		BBGameState->SetGameState(NewState);
	}
	else
	{
		BBGameState = GetGameState<ABBGameState>();
		if (ensureAlways(BBGameState))
		{
			BBGameState->SetGameState(NewState);
		}
	}
}

EBBGameState ABBGameMode::GetBBGameState()
{
	if(BBGameState)
	{
		return  BBGameState->GetGameState();
	}
	return EBBGameState::NULLState;
}

int ABBGameMode::GetCurrentPlayerCount()
{
	int count = 0;
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (PC && PC->GetPawn())
		{
			count++;
			//APawn* MyPawn = PC->GetPawn(); 
		}
	}
	return count;
}


bool ABBGameMode::GetAnyBaseBuilderAlive()
{
	for (APlayerController* baseBuilderController : baseBuilders)
	{
		ABBCharacter* baseBuilderCharacter = Cast<ABBCharacter>(baseBuilderController->GetCharacter());
		if(baseBuilderCharacter)
		{
			if(baseBuilderCharacter->bDied == false)
			{
				return true;
			}
		}
	}
	return  false;
}

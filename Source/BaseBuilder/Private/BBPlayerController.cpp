// Fill out your copyright notice in the Description page of Project Settings.


#include "BBPlayerController.h"

#include "BBCharacter.h"
#include "BBAttackerCharacterBase.h"
#include "Net/UnrealNetwork.h"

void ABBPlayerController::ResetRespawnTimer()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_RespawnTimer);
}

void ABBPlayerController::RespawnTimer(FVector spawnPos, FRotator spawnRot, TSubclassOf<ABBCharacter> characterClass)
{
	if(IsPendingKillPending() )
	{
		return;
	}

	
	//StartSpot
	TimerHandle_RespawnDelegate.BindUFunction(this, FName("RespawnCharacter"), spawnPos, spawnRot, characterClass);
	//GetWorldTimerManager().SetTimer(TimerHandle_RespawnTimer, TimerHandle_RespawnDelegate,RespawnTime, false);
	GetWorldTimerManager().SetTimer(TimerHandle_RespawnTimer, TimerHandle_RespawnDelegate,RespawnTime, false);
	
}

void ABBPlayerController::RespawnCharacter(FVector spawnPos, FRotator spawnRot, TSubclassOf<ABBCharacter> characterClass)
{
	if(BBCharacterType == EBBCharacterType::BaseAttacker)
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		
		ABBAttackerCharacterBase* CharacterToPosses = GetWorld()->SpawnActor<ABBAttackerCharacterBase>(characterClass, spawnPos, spawnRot, SpawnParameters);
		if(CharacterToPosses)
		{
			//SetBaseAttackerCharacterOptions(CharacterToPosses);
			this->Possess(CharacterToPosses);
			CharacterToPosses->SetActorRotation(spawnRot);

		}
	}
	else
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		
		ABBAttackerCharacterBase* CharacterToPosses = GetWorld()->SpawnActor<ABBAttackerCharacterBase>(characterClass, spawnPos, spawnRot, SpawnParameters);
		if(CharacterToPosses)
		{
			//SetBaseAttackerCharacterOptions(CharacterToPosses);
			this->Possess(CharacterToPosses);
			CharacterToPosses->SetActorRotation(spawnRot);
		}
	}
}

void ABBPlayerController::SetBBCharacterType(EBBCharacterType CharacterType)
{
	
}

void ABBPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABBPlayerController, BBCharacterType);
}

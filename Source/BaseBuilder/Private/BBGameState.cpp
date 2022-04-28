// Fill out your copyright notice in the Description page of Project Settings.


#include "BBGameState.h"
#include "Net/UnrealNetwork.h"



void ABBGameState::SetGameState(EBBGameState NewState)
{
	if (HasAuthority())
	{
		EBBGameState OldState = BBGameState;

		BBGameState = NewState;

		//Call on server
		OnRep_BBGameState(OldState);
	}
}

EBBGameState ABBGameState::GetGameState()
{
	return BBGameState;
}

void ABBGameState::SetRemainingSecondsForCurrentState(float newSeconds)
{
	if (HasAuthority())
	{
		RemainingSecondsForCurrentState = newSeconds;
	}
}


void ABBGameState::OnRep_BBGameState(EBBGameState OldState)
{
	BBGameStateChanged(BBGameState, OldState);
}


void ABBGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABBGameState, BBGameState);
	DOREPLIFETIME(ABBGameState, RemainingSecondsForCurrentState);
}

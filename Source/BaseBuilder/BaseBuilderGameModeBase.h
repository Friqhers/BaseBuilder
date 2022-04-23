// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BaseBuilderGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class BASEBUILDER_API ABaseBuilderGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
};

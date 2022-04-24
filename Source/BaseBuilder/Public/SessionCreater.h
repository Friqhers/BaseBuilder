// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class BASEBUILDER_API SessionCreater
{
public:
	SessionCreater();
	~SessionCreater();

	UFUNCTION(BlueprintCallable)
	void SetUseLobbiesIfAvailable();
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BBBaseBlock.generated.h"

class ABBCharacter;

USTRUCT()
struct FMoveObject
{
	GENERATED_BODY()

public:

	UPROPERTY()
	AActor* NewMovableObject;
	
	UPROPERTY()
	FVector TeleportDest;
	
	FVector_NetQuantize test;
};

UCLASS()
class BASEBUILDER_API ABBBaseBlock : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABBBaseBlock();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* BaseBlockMesh;


	void UpdatePosition();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerUpdatePosition();

	UPROPERTY(ReplicatedUsing = OnRep_ObjectMovingChange)
	FMoveObject CurrentObjectAndMovingMode;

	UFUNCTION()
	void OnRep_ObjectMovingChange();
	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite, Category = "BaseBlock", Replicated)
	bool BlockIsActive = false;

	UPROPERTY(BlueprintReadWrite, Category = "BaseBlock", Replicated)
	ABBCharacter* OwnerCharacter = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Baseblock options")
	int defaultDistanceBetween = 200;
};

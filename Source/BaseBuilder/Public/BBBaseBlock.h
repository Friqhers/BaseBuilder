// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BBBaseBlock.generated.h"

class ABBCharacter;


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

	UFUNCTION(Reliable, Server)
	void ServerUpdatePosition();
	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite, Category = "BaseBlock", Replicated)
	bool BlockIsActive = false;

	UPROPERTY(BlueprintReadWrite, Category = "BaseBlock", Replicated)
	ABBCharacter* OwnerCharacter = nullptr;

	
public:
	UPROPERTY(ReplicatedUsing=OnRep_collisionEnabled)
	bool collisionEnabled = true;
protected:
	UFUNCTION()
	void OnRep_CollisionEnabled();
};

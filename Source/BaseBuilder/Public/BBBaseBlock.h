// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BBBaseBlock.generated.h"

class ABBBuilderCharacterBase;

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
	

	UPROPERTY(EditAnywhere, Category = "Visual Effects")
	UMaterialInterface* Material;
	
	UPROPERTY(BlueprintReadWrite)
	UMaterialInstanceDynamic* DynamicMaterial;
	
	FVector lastPosition;

	FTransform defaultTransform;
	
	void UpdatePosition();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* BaseBlockMesh;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(ReplicatedUsing = OnRep_BlockIsActive)
	bool BlockIsActive = false;

	UPROPERTY(BlueprintReadWrite, Category = "BaseBlock", Replicated)
	ABBBuilderCharacterBase* OwnerCharacter = nullptr;
	
	UPROPERTY(ReplicatedUsing=OnRep_collisionEnabled)
	bool collisionEnabled = true;

	UPROPERTY(Replicated, BlueprintReadOnly)
	FVector blockTeleportPosition;

	UPROPERTY(ReplicatedUsing = OnRep_BlockLockChanges)
	bool BlockIsLockedToOwner = false;

	UPROPERTY(ReplicatedUsing = OnRep_BlockIsSynced)
	bool BlockIsSynced = true;

	UFUNCTION()
	void OnRep_BlockLockChanges();

	UFUNCTION()
	void OnRep_BlockIsSynced();

	UFUNCTION()
	void OnRep_BlockIsActive();
	
	void Lock(ABBBuilderCharacterBase* ownerPlayer);
	void Unlock();

	virtual void Reset() override;

	
protected:
	UFUNCTION()
	void OnRep_CollisionEnabled();
	
};

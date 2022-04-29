// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BBHealthComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FOnHealthChangedSignature, class UBBHealthComponent*, InHealthComp, float, Health, float, HealthDelta, const class UDamageType*, DamageType, class AController*, InstigatedBy, AActor*, DamageCauser);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BASEBUILDER_API UBBHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBBHealthComponent();

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHealthChangedSignature OnHealthChanged;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HealthComponent")
	uint8 TeamNum;
	

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnRep_CurrentHealth(float OldHealth);

	UFUNCTION()
	void HandleTakeAnyDamage( AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
	
	//Health related
	UPROPERTY(ReplicatedUsing = OnRep_CurrentHealth, BlueprintReadOnly, Category = "HealthComponent")
	float CurrentHealth;

	UPROPERTY(BlueprintReadOnly, Category = "HealthComponent")
	float MaxHealth;
	
	bool bIsDead;

	//Armor related
	UPROPERTY(EditDefaultsOnly, Category = "HealthComponent")
	bool bHasArmor;

	UPROPERTY(BlueprintReadOnly, Category = "HealthComponent")
	float CurrentArmor;

	UPROPERTY(BlueprintReadOnly, Category = "HealthComponent")
	float MaxArmor;

public:

	float GetCurrentHealth() const;
	float GetCurrentArmor() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "HealthComponent")
	static bool IsFriendly(AActor* ActorA, AActor* ActorB);
	
};

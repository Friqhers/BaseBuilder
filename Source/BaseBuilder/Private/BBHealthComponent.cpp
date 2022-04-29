// Fill out your copyright notice in the Description page of Project Settings.


#include "BBHealthComponent.h"

#include "BBGameMode.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UBBHealthComponent::UBBHealthComponent()
{
	MaxHealth = 100.0f;
	MaxArmor = 50.0f;
	bIsDead = false;
	TeamNum = 255;
	SetIsReplicatedByDefault(true);
}


// Called when the game starts
void UBBHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	if(GetOwner()->HasAuthority())
	{
		AActor* OwnerActor = GetOwner();
		if(OwnerActor)
		{
			OwnerActor->OnTakeAnyDamage.AddDynamic(this, &UBBHealthComponent::HandleTakeAnyDamage);
		}
	}
	CurrentHealth = MaxHealth;
	if(bHasArmor)
	{
		CurrentArmor = MaxArmor;
	}
	
}

void UBBHealthComponent::OnRep_CurrentHealth(float OldHealth)
{
	float Damage = CurrentHealth - OldHealth;
	OnHealthChanged.Broadcast(this, CurrentHealth, Damage, nullptr, nullptr, nullptr);
}

void UBBHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	AController* InstigatedBy, AActor* DamageCauser)
{
	if(Damage <= 0 || bIsDead)
	{
		return;
	}

	//if it is friendly or trying to damage himself
	if(DamageCauser != DamagedActor && IsFriendly(DamagedActor, DamageCauser) == true)
	{
		return;
	}

	if(bHasArmor)
	{
		if(CurrentArmor <= 0.0f)
		{
			CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.0f, MaxHealth);
			UE_LOG(LogTemp, Log, TEXT("Health changed: %s"), *FString::SanitizeFloat(CurrentHealth));
			OnHealthChanged.Broadcast(this, CurrentHealth, Damage, DamageType, InstigatedBy, DamageCauser);
		}
		else
		{
			float RemainingDamage = Damage - CurrentArmor;
			CurrentArmor = FMath::Clamp(CurrentArmor - Damage, 0.0f, MaxArmor);
			UE_LOG(LogTemp, Log, TEXT("Armor changed: %s"), *FString::SanitizeFloat(CurrentArmor));

			if(CurrentArmor == 0.0f)
			{
				//armor broken
				
			}

			if(RemainingDamage > 0.0f)
			{
				CurrentHealth = FMath::Clamp(CurrentHealth - RemainingDamage, 0.0f, MaxHealth);
				UE_LOG(LogTemp, Log, TEXT("Health changed: %s"), *FString::SanitizeFloat(CurrentHealth));
				OnHealthChanged.Broadcast(this, CurrentHealth, Damage, DamageType, InstigatedBy, DamageCauser);
			}

		}
		
	}
	else
	{
		CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.0f, MaxHealth);
		UE_LOG(LogTemp, Log, TEXT("Health changed: %s"), *FString::SanitizeFloat(CurrentHealth));
		OnHealthChanged.Broadcast(this, CurrentHealth, Damage, DamageType, InstigatedBy, DamageCauser);
	}

	
	if(CurrentHealth <= 0.0f)
	{
		bIsDead = true;
	}

	
}

float UBBHealthComponent::GetCurrentHealth() const
{
	return CurrentHealth;
}

float UBBHealthComponent::GetCurrentArmor() const
{
	return CurrentArmor;
}

bool UBBHealthComponent::IsFriendly(AActor* ActorA, AActor* ActorB)
{
	if(ActorA == nullptr || ActorB == nullptr)
	{
		return false;
	}

	UBBHealthComponent* HealthCompA = Cast<UBBHealthComponent>(ActorA->GetComponentByClass(UBBHealthComponent::StaticClass()));
	UBBHealthComponent* HealthCompB = Cast<UBBHealthComponent>(ActorB->GetComponentByClass(UBBHealthComponent::StaticClass()));

	if(HealthCompA == nullptr || HealthCompB == nullptr)
	{
		return false;
	}

	return HealthCompA->TeamNum == HealthCompB->TeamNum;
}


void UBBHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UBBHealthComponent, CurrentHealth);
}

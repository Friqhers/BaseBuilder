// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BBDoorBase.generated.h"

UENUM(BlueprintType)
enum class EDoorState : uint8
{
	Closed,

	Opened,

	Opening,

	Closing,

};

UCLASS()
class BASEBUILDER_API ABBDoorBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABBDoorBase();

	virtual void Reset() override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual  void Tick(float DeltaSeconds) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Door")
	float OpenTime;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "Door")
	float CloseTime;

	UPROPERTY(BlueprintReadWrite, Category = "Door")
	EDoorState DoorState;

	FTimerHandle TimerHandle_WaitBeforeOpen;

	FTimerHandle TimerHandle_WaitBeforeClose;

public:	
	// Called every frame
	//virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Door")
	void OpenDoor();

	UFUNCTION(BlueprintImplementableEvent, Category = "Door")
	void CloseDoor();

	UFUNCTION(BlueprintCallable, Category = "Door")
	void CheckBeforeOpening();

	UFUNCTION(BlueprintCallable, Category = "Door")
	void CheckBeforeClosing();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	bool bShouldKeepOpen = false;

};

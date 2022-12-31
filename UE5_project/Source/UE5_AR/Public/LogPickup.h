// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Resource.h"
#include "LogPickup.generated.h"

//Child class of AResource. It defines a single pickupable log
UCLASS()
class UE5_AR_API ALogPickup : public AResource
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALogPickup();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
};

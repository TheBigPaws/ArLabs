// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Resource.h"
#include "LogPile.generated.h"

//Child class of AResource. It defines a pile pickupable logs. There are 14 in the provided mesh
UCLASS()
class UE5_AR_API ALogPile : public AResource
{
	GENERATED_BODY()
public:
	// Sets default values for this actor's properties
	ALogPile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

};

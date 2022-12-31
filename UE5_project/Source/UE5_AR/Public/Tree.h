// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Resource.h"
#include "LogPickup.h"
#include "LogPile.h"
#include "TextActor.h"
#include "Tree.generated.h"


/**
 * 
 */
UCLASS()
class UE5_AR_API ATree : public AResource
{
	GENERATED_BODY()
public:
	// Sets default values for this actor's properties
	ATree();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	//tree attributes
	float stateIncrementor;
	bool showHealth = false;
	int RSShealth = 5;
	int logsInTree = 5;
	FString state_ = "Idle";
	FVector shakingDesiredRPY;

	//health text variables
	float textDispDist = 0.0f;
	ATextActor* healthTextRef;

};

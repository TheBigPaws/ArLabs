// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameModeBase.h"

#include "HelloARManager.h"

#include "CustomGameMode.generated.h"

//Forward Declarations
class ACustomARPawn;


UCLASS()
class UE5_AR_API ACustomGameMode : public AGameModeBase
{
	GENERATED_BODY()
private:

	FTimerHandle Ticker;

public:

	//base functions

	ACustomGameMode();
	virtual ~ACustomGameMode() = default;

	virtual void StartPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	//Phone-relevant tap-ray casting functions
	bool WorldHitTest(FVector2D screenTouchPos, FHitResult& hitResult);


	//player previous search position (thus avoid continuously searching in one place)
	FIntVector2 last_searchPos = FIntVector2( - 17, -17);

	//spawning positions references
	TArray<FVector> assigned_spawn_pos;
	TArray<FVector> spawn_pos_AVAILABle;

	//parameters adjustable in UI
	UPROPERTY(Category = "spawnParam", EditAnywhere, BlueprintReadWrite)
	int32 spawnDensity = 180;
	UPROPERTY(Category = "spawnParam", EditAnywhere, BlueprintReadWrite)
	int32 spawnDistance = 370;

	//function called by UI
	UFUNCTION(BlueprintCallable, Category = "DensitySeting")
		void setSpawnDensity(int32 newSpawnDensity);

	//tick functions
	void handleResourceSpawning(float dt);
	void update_Spawn_Positions();

	//returns nearby locations on an imaginary grid specified by function parameters
	TArray<FIntVector2> getAllNearbyGridPositions(int32 spawnDensity_, int32 spawnDistance_, FIntVector2 middlePos_);

	//spawn a random resource on a given a position
	void spawnRandomResource(FVector s_position);

	//returns what position on the grid this would be treated as (always lower X and Y)
	FIntVector2 gridCutoff(FIntVector2 pos_, int32 gridSize);

	UFUNCTION(BlueprintCallable, Category = "Interact")
		void setInteractMode(FString setter);

	 UPROPERTY(Category="states",EditAnywhere,BlueprintReadWrite)
	 FString interactionState = "Interact";


	 AHelloARManager* Manager = nullptr;
	 ACustomARPawn* pawnRef = nullptr;
	 float spawnTimer = 0.2f;
};

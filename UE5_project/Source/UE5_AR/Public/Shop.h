// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Shop.generated.h"

UCLASS()
class UE5_AR_API AShop : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShop();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//base components
	UPROPERTY(Category = "myCategory", VisibleAnywhere, BlueprintReadWrite)
		USceneComponent* SceneComponent;

	UPROPERTY(Category = "myCategory", VisibleAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* StaticMeshComponent;

	//upgrade logic
	int upgradeTier = 1;
	UPROPERTY(Category = "Resources", VisibleAnywhere, BlueprintReadWrite)
		int Wood_COST = 10;
	UPROPERTY(Category = "Resources", VisibleAnywhere, BlueprintReadWrite)
		int Stone_COST = 10;
	UPROPERTY(Category = "Resources", VisibleAnywhere, BlueprintReadWrite)
		int Iron_COST = 0;
	UPROPERTY(Category = "Resources", VisibleAnywhere, BlueprintReadWrite)
		int Mithril_COST = 0;

	//upgrade functions
	bool CanUpgradeTool(int playerWood, int playeStone, int playerIron, int playerMithril);
	void TierUp();



};

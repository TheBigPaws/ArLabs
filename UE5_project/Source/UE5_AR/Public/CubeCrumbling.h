// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CreumbleWallPiece.h"
#include "CubeCrumbling.generated.h"

UCLASS()
class UE5_AR_API ACubeCrumbling : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACubeCrumbling();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	//float destroyPercentge = 0.2f;

	UPROPERTY(Category = "myCategory", VisibleAnywhere, BlueprintReadWrite)
		int crumbleWallSize = 7;
	//UPROPERTY(Category = "myCategory", VisibleAnywhere, BlueprintReadWrite)
	//	float destroyPercentage =  0.2f;
	UPROPERTY(Category = "myCategory", VisibleAnywhere, BlueprintReadWrite)
		USceneComponent* SceneComponent;

	UPROPERTY(Category = "myCategory", VisibleAnywhere, BlueprintReadWrite)
	
	
	TArray <ACreumbleWallPiece *> crumblePieceArray;
	TArray <ACreumbleWallPiece *> solidPieceArray;

	//for gradual spawning
	float CCscale;


	UStaticMesh* CubeMesh_;



	UFUNCTION(BlueprintCallable, Category = "Crumbling")
	void partlyCrumble(int miningStrength);
	//UFUNCTION(BlueprintCallable, Category = "Crumbling")
	void setupCrumbleWall(int sideDimension = 5);
};

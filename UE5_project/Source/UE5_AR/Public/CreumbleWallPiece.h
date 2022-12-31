// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CreumbleWallPiece.generated.h"


//forward declaration to parent
class ACubeCrumbling;



//this class is utilised by CubeCrumbling
UCLASS()
class UE5_AR_API ACreumbleWallPiece : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACreumbleWallPiece();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//base components
	UPROPERTY(Category = "myCategory", VisibleAnywhere, BlueprintReadWrite)
		USceneComponent* myRootComponent;
	UStaticMeshComponent* cubeMeshComponent;
	UStaticMesh* CubeMesh_;
	UMaterialInstanceDynamic* rockMat;

	//reference to player
	ACubeCrumbling* parentRef;
	
	//simple behaviour variables/functuions
	float lifeTime = 2.0f;
	float scaleDecrementor;
	bool dynamic = false;
	void updateMe(float dt);
};

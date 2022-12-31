// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OpeningDoor.h"
#include "CubeCrumbling.h"
#include "MineableStone.h"

#include "StencilBox.generated.h"


//AR inspired stencil class that simulates virtual space inside a solid, real-world mass.
//while currently the object is a box, with ore/items inside, this logic can be further 
//explored to create believable lakes, holes, portals or spaces such as inside of a house, in AR

UCLASS()
class UE5_AR_API AStencilBox : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AStencilBox();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	//AOpeningDoor* doorOne;
	//AOpeningDoor* doorTwo;

	//material for items inside, as well as for the front of the box which acts as the stencil through what everything is rendered.
	UMaterialInstanceDynamic* boxFrontMat;
	UMaterialInstanceDynamic* boxInsideMat;

	UMaterial* StoredMaterial;
	UMaterial* StoredMaterialTWO;

	//base components
	UPROPERTY(Category = "myCategory", VisibleAnywhere, BlueprintReadWrite)
	TArray<UStaticMeshComponent * >BoxWalls;

	UPROPERTY(Category = "myCategory", VisibleAnywhere, BlueprintReadWrite)
		USceneComponent* SceneComponent;

	//have a reference to the ore thats inside for the scan function
	UPROPERTY(Category = "UIrelevant", EditAnywhere, BlueprintReadWrite)
	FString myOreSTRING;

	//given a type of a doore, this function will attach a CrumblingWall or a OpeningDoor to the entrance of the box, as well as spawn the desired type and number of ore veins inside
	void addDoor(int type, int crumbleSize, int oreInside, FString oreType);

};

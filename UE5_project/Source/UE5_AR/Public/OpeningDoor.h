// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OpeningDoor.generated.h"

UCLASS()
class UE5_AR_API AOpeningDoor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AOpeningDoor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	//default components
	UPROPERTY(Category = "myCategory", VisibleAnywhere, BlueprintReadWrite)
		USceneComponent* SceneComponent;

	UPROPERTY(Category = "myCategory", VisibleAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* StaticMeshComponent;

	//reference to the other door
	AOpeningDoor* otherDoor;

	float openProgress;
	bool opening;


	//material components
	UMaterialInstanceDynamic* mat_INST;
	UMaterial* mat;

	//reference to the box they're blocking
	AActor* stencilBoxRef;
};

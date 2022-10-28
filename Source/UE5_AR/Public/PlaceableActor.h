// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


//#include "CustomARPawn.h"
#include "GameFramework/Actor.h"
#include "PlaceableActor.generated.h"

class UARPin;

UCLASS()
class UE5_AR_API APlaceableActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlaceableActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	//ACustomARPawn * pawnRef;

	UPROPERTY(Category = "myCategory", VisibleAnywhere, BlueprintReadWrite)
		USceneComponent* SceneComponent;

	UPROPERTY(Category = "myCategory", VisibleAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* StaticMeshComponent;

	UARPin* PinComponent;

	bool nearby = false;
	bool selected = false;

	UMaterialInstanceDynamic* onMat;
	UMaterialInstanceDynamic* offMat;
	UMaterialInstanceDynamic* selectedMat;

	FVector myScale;
	FVector myPos;

	float expander = 10.0f;
};

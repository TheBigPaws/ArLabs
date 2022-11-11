// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OpeningDoor.h"

#include "StencilBox.generated.h"

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
	AOpeningDoor* doorOne;
	AOpeningDoor* doorTwo;

	UMaterialInstanceDynamic* onMat;

	UPROPERTY(Category = "myCategory", VisibleAnywhere, BlueprintReadWrite)
	TArray<UStaticMeshComponent * >BoxWalls;

	UPROPERTY(Category = "myCategory", VisibleAnywhere, BlueprintReadWrite)
		USceneComponent* SceneComponent;

};

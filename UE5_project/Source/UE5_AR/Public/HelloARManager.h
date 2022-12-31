// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "GameFramework/Actor.h"
#include "HelloARManager.generated.h"


class UARSessionConfig;
class AARPlaneActor;
class UARPlaneGeometry;


struct possible_AR_pos_return {
public:
	possible_AR_pos_return(bool exists_, float Zpos_) {
		exists = exists_;
		Zpos = Zpos_;
	}
	bool exists;
	float Zpos;
};

UCLASS()
class UE5_AR_API AHelloARManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHelloARManager();

	struct SpawnPosAndUpV {
	public:
		FVector world_Position;
		FVector world_UP_Vector;
	};


	


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Create a default Scene Component
	UPROPERTY(Category = "SceneComp", VisibleAnywhere, BlueprintReadWrite)
		USceneComponent* SceneComponent;

	//---------AR relevant functions:

	//custom function to reduce the unnecessary overlapped planes
	UFUNCTION(BlueprintCallable, Category = "Planes")
		bool combinePlanes();
	float combinePlanesTimer = 15.0f;

	// succesfully resets the AR session
	UFUNCTION(BlueprintCallable, Category = "resetAR")
	void resetAR();

	//---------spawning position check functions:
	
	//returns whether (and where) this XY position is on an AR plane
	possible_AR_pos_return griXYExistsOnAPlane(FIntVector2 position);

	//record any vertical planes that are a possible spawn place
	void noteValidVerticalLocs();

	float SpawnDistance = 200.0f;

	TArray<SpawnPosAndUpV> validSpawnPlacesVERT;
	TArray<FVector> valid_USED_SpawnPlacesVERT;

protected:

	//---------provided functionality:

	// Updates the plane actors on every frame as long as the AR Session is running
	void UpdatePlaneActors();

	AARPlaneActor* SpawnPlaneActor();
	FColor GetPlaneColor(int Index);

	void ResetARCoreSession();

	// Configuration file for AR Session
	UARSessionConfig * Config;

	//Base plane actor for geometry detection
	AARPlaneActor* PlaneActor;


	//Map of geometry planes
	TMap<UARPlaneGeometry*, AARPlaneActor*> PlaneActors;

	//Index for plane colours adn array of colours
	int PlaneIndex = 0;
	TArray<FColor> PlaneColors;
	
	
	
};

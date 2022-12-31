// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TextRenderComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"
#include "TextActor.generated.h"

//class created to display text in the game world.
//this has many contextual uses
UCLASS()
class UE5_AR_API ATextActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATextActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//base component
	UTextRenderComponent* myTextComponent;

	//attributes
	FString myString;
	float lifeLeft;
	FVector velocity;
	bool timeDestroyable = true;

	//necessary for billboarding
	UCameraComponent* camRef;

	//setup function for ease of use
	void setupText(FString dispText, float howLongToDisplay, FColor color, float size, UCameraComponent* PawnCamRef, FVector moveVelocity = FVector(0, 0, 0));


};

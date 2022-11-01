// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
//#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "PlaceableActor.h"
#include "GameFramework/Pawn.h"
#include "ARSessionConfig.h"
#include "ARBlueprintLibrary.h"
#include "CustomARPawn.generated.h"

class UCameraComponent;

UCLASS()
class UE5_AR_API ACustomARPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACustomARPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnScreenTouch(const ETouchIndex::Type FingerIndex, const FVector ScreenPos);
	virtual void OnScreenHeld(const ETouchIndex::Type FingerIndex, const FVector ScreenPos);

	UARSessionConfig* Config;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//virtual void OnScreenTouch(const ETouchIndex::Type fingerIndex, const FVector screenPos);

	UPROPERTY(Category = "myCategory", VisibleAnywhere, BlueprintReadWrite)
		USceneComponent* SceneComponent;

	UPROPERTY(Category = "myCategory", VisibleAnywhere, BlueprintReadWrite)
		UCameraComponent* CameraComponent;

	bool WorldHitTest(FVector2D screenTouchPos, FHitResult & hitResult);

	void handleImageRecognition();

	APlaceableActor* selectedActor_;

	bool bGoghFound = false;
	bool bWorldFound = false;
	AActor* GoghCube;
	AActor* WorldCube;

};

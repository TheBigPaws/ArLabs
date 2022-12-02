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
//UENUM()
//enum ClickState{MOVE,SELECT,PLACE};
UENUM()
enum ClickState
{
	MOVE    UMETA(DisplayName = "MOVE"),
	SELECT  UMETA(DisplayName = "SELECT"),
	PLACE   UMETA(DisplayName = "PLACE"),
};

UCLASS()
class UE5_AR_API ACustomARPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACustomARPawn();
	UPROPERTY(Category = "myCategory", VisibleAnywhere, BlueprintReadWrite)
	TEnumAsByte<ClickState> myClickState;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnScreenTouch(const ETouchIndex::Type FingerIndex, const FVector ScreenPos);
	virtual void OnScreenHeld(const ETouchIndex::Type FingerIndex, const FVector ScreenPos);

	UARSessionConfig* Config;

	float desktopMSspeed = 60.0f;

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

	UFUNCTION(BlueprintCallable)
		void setClickMode(TEnumAsByte<ClickState> StateSetter) {
		myClickState = StateSetter;
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, "Switched State!");
	}

	

	void handleImageRecognition();

	APlaceableActor* selectedActor_;

	bool bGoghFound = false;
	bool bWorldFound = false;
	AActor* GoghCube;
	AActor* WorldCube;

	void moveRight(float amount);
	void moveUp(float amount);
	void moveForward(float amount);
	void LookRight(float amount);
	void LookUp(float amount);
	void selectVisibilityOfAllPlanes(bool visibility);
};

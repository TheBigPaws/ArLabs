// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
//#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "ARSessionConfig.h"
#include "Tree.h"
#include "CustomGameMode.h"
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

	//touch and hold functions
	virtual void OnScreenTouch(const ETouchIndex::Type FingerIndex, const FVector ScreenPos);
	virtual void OnScreenHeld(const ETouchIndex::Type FingerIndex, const FVector ScreenPos);

	UARSessionConfig* Config;

public:	

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//default components
	UPROPERTY(Category = "myCategory", VisibleAnywhere, BlueprintReadWrite)
		USceneComponent* SceneComponent;

	UPROPERTY(Category = "myCategory", VisibleAnywhere, BlueprintReadWrite)
		UCameraComponent* CameraComponent;

	ACustomGameMode* GM_ref;

	//sounds made by player interactions
	USoundWave* treeChopSound;
	USoundWave* shopUpgrade;
	USoundWave* pickupSound;
	USoundWave* miningSound;
	USoundWave* TreeFallngSound;

	//in-game damage text
	void spawnDamageTextAtPos(bool isCrit,int damageTaken,FVector position);

	//------------player variables

	UPROPERTY(Category = "Resources", VisibleAnywhere, BlueprintReadWrite)
	FString currentToolLevel = FString("Wood");

	int miningStrength = 1;
	float interactionReach = 300.0f;

	UPROPERTY(Category = "Resources", VisibleAnywhere, BlueprintReadWrite)
		int Wood_In_Inventory = 0;
	UPROPERTY(Category = "Resources", VisibleAnywhere, BlueprintReadWrite)
		int Stone_In_Inventory = 0;
	UPROPERTY(Category = "Resources", VisibleAnywhere, BlueprintReadWrite)
		int Iron_In_Inventory = 0;
	UPROPERTY(Category = "Resources", VisibleAnywhere, BlueprintReadWrite)
		int Mithril_In_Inventory = 0;

	//------------shop building functions & variables

	UFUNCTION(BlueprintCallable, Category = "Planes")
	void constructShopPreview();

	UFUNCTION(BlueprintCallable, Category = "Planes")
	void destroyShopPreview();

	void moveShopPreview();

	float initScreenXforRotate = 0.0f;

	AActor* buildPreview = nullptr;

};

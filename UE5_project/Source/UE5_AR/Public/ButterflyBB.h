// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Camera/CameraComponent.h"

#include "ButterflyBB.generated.h"

UCLASS()
class UE5_AR_API AButterflyBB : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AButterflyBB();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//base components
	UPROPERTY(Category = "myCategory", VisibleAnywhere, BlueprintReadWrite)
		USceneComponent* SceneComponent_b;

	UPROPERTY(Category = "myCategory", VisibleAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* StaticMeshComponent_b;

	UMaterialInstanceDynamic* mat_INST;
	UMaterial* mat;

	//camera reference for billboarding
	UCameraComponent* camRef;

	
	//simple behaviour values
	FString flyState = FString("Idle");

	float CLOSE_STOP_TH = 100.0f;
	float START_FOLLOWING_TH = 500.0f;
	float idleDirTimeLeft = 3.0f;
	float flySpeed = 3.0f;

	FVector flyDir;

	TArray<FColor> choices_;

};

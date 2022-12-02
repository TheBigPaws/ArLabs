// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CubeCrumbling.generated.h"

UCLASS()
class UE5_AR_API ACubeCrumbling : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACubeCrumbling();
	struct CrumblePiece
	{
	public:
		UStaticMeshComponent* StaticMeshComponent;
		float lifeTime = 2.0f;
		float scaleDecrementor;
		bool colliding = false;
		void updateMe(float dt) {
			float currScale = StaticMeshComponent->GetComponentScale().X;
			currScale -= scaleDecrementor * dt;
			StaticMeshComponent->SetWorldScale3D(FVector(currScale, currScale, currScale));
			lifeTime -= dt;

			
		}
	};
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	float destroyPercentge = 0.2f;

	UPROPERTY(Category = "myCategory", VisibleAnywhere, BlueprintReadWrite)
		int crumbleWallSize = 7;
	UPROPERTY(Category = "myCategory", VisibleAnywhere, BlueprintReadWrite)
		float destroyPercentage =  0.2f;
	UPROPERTY(Category = "myCategory", VisibleAnywhere, BlueprintReadWrite)
		USceneComponent* SceneComponent;

	UPROPERTY(Category = "myCategory", VisibleAnywhere, BlueprintReadWrite)
	TArray <UStaticMeshComponent*> StaticMeshComponentsARRAY;
	TArray <CrumblePiece> crumblePieceArray;

	UFUNCTION(BlueprintCallable, Category = "Crumbling")
	void partlyCrumble();
};

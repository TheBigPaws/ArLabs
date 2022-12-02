// Fill out your copyright notice in the Description page of Project Settings.


#include "CubeCrumbling.h"

#include "Kismet/GameplayStatics.h"
#include <string>
#include "Components/PrimitiveComponent.h"
#include "GameFramework/DefaultPawn.h"
#include "Engine/EngineTypes.h"

// Sets default values
ACubeCrumbling::ACubeCrumbling()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComponent);

	auto MeshAsset = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	float cubeScale = 100.0f / crumbleWallSize;
	cubeScale /= 100.0f;
	for (int j = 0; j < crumbleWallSize; j++) {
		for (int i = 0; i < crumbleWallSize; i++) {

			std::string name_ = "StaticMeshComponent_";
			name_ += std::to_string(i).c_str();
			name_ += std::to_string(j).c_str();
			StaticMeshComponentsARRAY.Add(CreateDefaultSubobject<UStaticMeshComponent>(name_.c_str()));
			StaticMeshComponentsARRAY.Last()->SetupAttachment(SceneComponent);
			StaticMeshComponentsARRAY.Last()->SetStaticMesh(MeshAsset.Object);


			StaticMeshComponentsARRAY.Last()->SetRelativeLocation(FVector(0.0f, cubeScale * 100.0f * i, cubeScale * j * 100.0f));
			StaticMeshComponentsARRAY.Last()->SetWorldScale3D(FVector(0.2f, cubeScale, cubeScale));

			StaticMeshComponentsARRAY.Last()->SetCollisionProfileName("BlockAll");
			StaticMeshComponentsARRAY.Last()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

			//StaticMeshComponentsARRAY.Last()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			//StaticMeshComponentsARRAY.Last()->SetSimulatePhysics(true);
		}
	}
}

// Called when the game starts or when spawned
void ACubeCrumbling::BeginPlay()
{
	Super::BeginPlay();
	//partlyCrumble();
}

// Called every frame
void ACubeCrumbling::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (crumblePieceArray.Num() > 0) {
		for (int i = 0; i < crumblePieceArray.Num(); i++) {


			crumblePieceArray[i].updateMe(DeltaTime);
			if (crumblePieceArray[i].lifeTime <= 0.0f) {
				crumblePieceArray[i].StaticMeshComponent->DestroyComponent();
				crumblePieceArray.RemoveAt(i);
				break;
			}

		}
	}
}

void ACubeCrumbling::partlyCrumble() {

	for (int i = 0; i < (crumbleWallSize * crumbleWallSize * destroyPercentage); i++) {

		if (StaticMeshComponentsARRAY.Num() == 0) {
			return;
		}
		else {
			CrumblePiece newCP;
			newCP.lifeTime = FMath::FRandRange(0.9f, 1.4f);
			
			int pos_;
			pos_ = FMath::RandRange(0, StaticMeshComponentsARRAY.Num() - 1);
			newCP.StaticMeshComponent = StaticMeshComponentsARRAY[pos_];
			newCP.scaleDecrementor = newCP.StaticMeshComponent->GetComponentScale().X / 2.0f * newCP.lifeTime;
			newCP.StaticMeshComponent->SetSimulatePhysics(true);
			newCP.StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			FVector FW = this->GetActorForwardVector();
			newCP.StaticMeshComponent->SetMassOverrideInKg(NAME_None, 1.0f);
			float init_scale = newCP.StaticMeshComponent->GetComponentScale().Y;
			newCP.StaticMeshComponent->SetWorldScale3D(FVector(0.15f, init_scale*0.8f, init_scale*0.8f));
			
			newCP.StaticMeshComponent->AddImpulse((FW.X * FVector(FMath::FRandRange(0.8f, 1.2f), FW.Y * FMath::FRandRange(0.8f, 1.2f),0.4f)) * FMath::FRandRange(200.0f, 400.0f));
			//newCP.StaticMeshComponent->SetCollisionProfileName("OverlapAllDynamic");

			//newCP.colliding = false;
			crumblePieceArray.Add(newCP);
			StaticMeshComponentsARRAY.RemoveAt(pos_);
		}

		
	}

}

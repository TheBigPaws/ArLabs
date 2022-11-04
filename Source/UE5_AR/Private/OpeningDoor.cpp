// Fill out your copyright notice in the Description page of Project Settings.


#include "OpeningDoor.h"

// Sets default values
AOpeningDoor::AOpeningDoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComponent);


	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(SceneComponent);

	auto MeshAsset = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	StaticMeshComponent->SetStaticMesh(MeshAsset.Object);
	
	openProgress = 0.0f;
}

// Called when the game starts or when spawned
void AOpeningDoor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AOpeningDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


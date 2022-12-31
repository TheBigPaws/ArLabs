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
	
	StaticMeshComponent->SetWorldScale3D(FVector(1.0f,0.5f,0.05f));

	static ConstructorHelpers::FObjectFinder<UMaterial> FoundMaterial_door(TEXT("Material'/Niagara/DefaultAssets/M_NiagaraUVDebug.M_NiagaraUVDebug'"));
	if (FoundMaterial_door.Succeeded())
	{
		mat = FoundMaterial_door.Object;
	}
	mat_INST = UMaterialInstanceDynamic::Create(mat, NULL);
	StaticMeshComponent->SetMaterial(0, mat_INST);

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


	if (opening) {
		openProgress += DeltaTime;
		if (openProgress >= 1) { //fully opened
			opening = false;
			openProgress = 1.0f;
		}

		
		if (openProgress < 0.5f) {
			this->AddActorLocalOffset(FVector(0.0f, 0.0f, 10 * DeltaTime)); //go up first
		}
		else {
			this->AddActorLocalOffset(FVector(0.0f, 100 * DeltaTime, 0.0f)); //then open to the side
		}

	}
}


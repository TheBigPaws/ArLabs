// Fill out your copyright notice in the Description page of Project Settings.


#include "goghCube.h"

// Sets default values
AgoghCube::AgoghCube()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComponent);


	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(SceneComponent);

	auto MeshAsset = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	StaticMeshComponent->SetStaticMesh(MeshAsset.Object);

	myMat = CreateDefaultSubobject<UMaterialInstanceDynamic>(TEXT("myMat"));
	static ConstructorHelpers::FObjectFinder<UMaterial> Material_s(TEXT("Material'/Game/Assets/Materials/redMat.redMat'"));
	UMaterial* settingMaterial_s = (UMaterial*)Material_s.Object;
	myMat = UMaterialInstanceDynamic::Create(settingMaterial_s, NULL);
	StaticMeshComponent->SetMaterial(0, myMat);

}

// Called when the game starts or when spawned
void AgoghCube::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AgoghCube::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


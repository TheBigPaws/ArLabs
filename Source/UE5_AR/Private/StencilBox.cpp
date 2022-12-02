// Fill out your copyright notice in the Description page of Project Settings.


#include "StencilBox.h"
#include <string>

// Sets default values
AStencilBox::AStencilBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;




	onMat = CreateDefaultSubobject<UMaterialInstanceDynamic>(TEXT("OnMaterial"));

	static ConstructorHelpers::FObjectFinder<UMaterial> Material_(TEXT("Material'/Game/Assets/Materials/greenMat.greenMat'"));
	UMaterial* settingMaterial_ = (UMaterial*)Material_.Object;
	onMat = UMaterialInstanceDynamic::Create(settingMaterial_, NULL);

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComponent);

	auto planeAsset = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Engine/BasicShapes/Plane.Plane'"));

	for (int i = 0; i < 5; i++) {

		std::string name_ = "StaticMeshComponent";
		name_ += std::to_string(i);
		BoxWalls.Add(CreateDefaultSubobject<UStaticMeshComponent>(FName(name_.c_str())));
		BoxWalls.Last()->SetupAttachment(SceneComponent);

		
		BoxWalls.Last()->SetStaticMesh(planeAsset.Object);
		BoxWalls.Last()->SetMaterial(0, onMat);

	}
	//BoxWalls[0]->SetRelativeLocation(FVector());


}

// Called when the game starts or when spawned
void AStencilBox::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AStencilBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


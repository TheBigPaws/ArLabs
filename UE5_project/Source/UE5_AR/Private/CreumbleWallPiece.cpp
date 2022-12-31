// Fill out your copyright notice in the Description page of Project Settings.


#include "CreumbleWallPiece.h"
#include "CubeCrumbling.h"
#include "Components/PrimitiveComponent.h"

// Sets default values
ACreumbleWallPiece::ACreumbleWallPiece()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	myRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(myRootComponent);

	auto MeshAsset = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	CubeMesh_ = MeshAsset.Object;

	
	std::string name_ = "StaticMeshComponent_";

	cubeMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(name_.c_str());
	cubeMeshComponent->SetupAttachment(myRootComponent);
	cubeMeshComponent->SetStaticMesh(CubeMesh_);

	
	cubeMeshComponent->SetCollisionProfileName("BlockAll");
	cubeMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);



	rockMat = CreateDefaultSubobject<UMaterialInstanceDynamic>(TEXT("RockMaterial"));
	static ConstructorHelpers::FObjectFinder<UMaterial> Material_(TEXT("Material'/Game/GreenwoodFantasyVillage/Materials/MasterMaterials/M_Triplanar.M_Triplanar'"));
	UMaterial* settingMaterial_ = (UMaterial*)Material_.Object;
	rockMat = UMaterialInstanceDynamic::Create(settingMaterial_, NULL);
	cubeMeshComponent->SetMaterial(0, rockMat);

}

// Called when the game starts or when spawned
void ACreumbleWallPiece::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACreumbleWallPiece::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//see if the stone shard is alive, in which case update its lifetime and slowly shrink it
void ACreumbleWallPiece::updateMe(float dt)
{
	if (dynamic) {
		float currScale = cubeMeshComponent->GetComponentScale().X;
		currScale -= scaleDecrementor * dt;
		cubeMeshComponent->SetWorldScale3D(FVector(currScale, currScale, currScale));
		lifeTime -= dt;
	}
}


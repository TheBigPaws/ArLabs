// Fill out your copyright notice in the Description page of Project Settings.


#include "StencilBox.h"
//#include "Components/PrimitiveComponent.h"
#include <string>

// Sets default values
AStencilBox::AStencilBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//get materials and base components
	static ConstructorHelpers::FObjectFinder<UMaterial> FoundMaterial(TEXT("Material'/Game/Assets/Materials/CaveMats/StencilBoxItemsInside.StencilBoxItemsInside'"));
	if (FoundMaterial.Succeeded())
	{
		StoredMaterial = FoundMaterial.Object;
	}
	boxInsideMat = UMaterialInstanceDynamic::Create(StoredMaterial, NULL);

	static ConstructorHelpers::FObjectFinder<UMaterial> FoundMaterialTWO(TEXT("Material'/Game/Assets/Materials/CaveMats/StencilBoxFront.StencilBoxFront'"));
	if (FoundMaterialTWO.Succeeded())
	{
		StoredMaterialTWO = FoundMaterialTWO.Object;
	}
	boxFrontMat = UMaterialInstanceDynamic::Create(StoredMaterialTWO, NULL);


	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComponent);

	auto planeAsset = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Engine/BasicShapes/Plane.Plane'"));

	//create and setup box sides
	for (int i = 0; i < 5; i++) {

		std::string name_ = "StaticMeshComponent";
		name_ += std::to_string(i);
		BoxWalls.Add(CreateDefaultSubobject<UStaticMeshComponent>(FName(name_.c_str())));
		BoxWalls.Last()->SetupAttachment(SceneComponent);
		BoxWalls.Last()->SetStaticMesh(planeAsset.Object);

		BoxWalls.Last()->SetMaterial(0, boxInsideMat);


	}
	//apply relative locations and rotations of the box sides
	BoxWalls[0]->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f));
	BoxWalls[0]->SetRelativeRotation(FRotator(0.0f, 0.0f, 180.0f));


	BoxWalls[1]->SetRelativeLocation(FVector(-50.0f,0.0f,0.0f));
	BoxWalls[1]->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
	
	BoxWalls[2]->SetRelativeLocation(FVector(0.0f,50.0f,0.0f));
	BoxWalls[2]->SetRelativeRotation(FRotator(0.0f, 0.0f, -90.0f));
	
	
	BoxWalls[3]->SetRelativeLocation(FVector(0.0f,-50.0f,0.0f));
	BoxWalls[3]->SetRelativeRotation(FRotator(0.0f, 0.0f, 90.0f));
	
	BoxWalls[4]->SetRelativeLocation(FVector(0.0f,0.0f,-50.0f));
	BoxWalls[4]->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	

	//setup stencil box front
	std::string name_ = "StaticMeshComponentFRONT";
	BoxWalls.Add(CreateDefaultSubobject<UStaticMeshComponent>(FName(name_.c_str())));
	BoxWalls.Last()->SetupAttachment(SceneComponent);
	BoxWalls.Last()->SetStaticMesh(planeAsset.Object);
	BoxWalls.Last()->SetMaterial(0, boxFrontMat);

	BoxWalls.Last()->SetCollisionProfileName("NoCollision");
	BoxWalls.Last()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	BoxWalls.Last()->SetRelativeLocation(FVector(50.0f, 0.0f, 0.0f));
	BoxWalls.Last()->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));

	BoxWalls.Last()->SetRenderInDepthPass(false);
	BoxWalls.Last()->SetRenderInMainPass(false);
	BoxWalls.Last()->SetRenderCustomDepth(true);
	BoxWalls.Last()->SetCustomDepthStencilValue(1);
	

}

// Called when the game starts or when spawned
void AStencilBox::BeginPlay()
{
	Super::BeginPlay();

	SetActorRotation(FVector(0,0,1).Rotation());
	addDoor(0,10,5,FString("Iron"));

}

// Called every frame
void AStencilBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AStencilBox::addDoor(int type, int crumbleSize,int oreInside, FString oreType) {
	const FActorSpawnParameters SpawnInfo;

	AOpeningDoor* DoorB  = nullptr;
	AOpeningDoor* DoorA	 = nullptr;

	switch (type) { //CREATING OPENING DOOR
	case 0:

		//spawn
		DoorA = GetWorld()->SpawnActor<AOpeningDoor>(GetActorLocation(), FRotator(0, 0, 0), SpawnInfo);
		DoorB = GetWorld()->SpawnActor<AOpeningDoor>(GetActorLocation(), FRotator(0, 0, 0), SpawnInfo);

		//set up door A
		DoorA->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
		DoorA->AddActorWorldOffset(this->GetActorForwardVector() * 50 * GetActorScale3D().X);
		DoorA->SetActorScale3D(this->GetActorScale3D());
		DoorA->SetActorRotation(GetActorUpVector().Rotation());

		//set up door B
		DoorB->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
		DoorB->AddActorWorldOffset(this->GetActorForwardVector() * 50 * GetActorScale3D().X);
		DoorB->SetActorScale3D(this->GetActorScale3D());
		DoorB->SetActorRotation(GetActorUpVector().Rotation());
		DoorB->AddActorLocalRotation(FRotator(0.0f, 180.0f, 0.0));

		//move the doors to the front of the box
		DoorA->AddActorLocalOffset(FVector(0.0f, 50.0f* GetActorScale3D().X, 0.0f));
		DoorB->AddActorLocalOffset(FVector(0.0f, 50.0f* GetActorScale3D().X, 0.0f));

		//link references
		DoorA->otherDoor = DoorB;
		DoorB->otherDoor = DoorA;
		DoorB->stencilBoxRef = this;
		DoorA->stencilBoxRef = this;
		break;

	case 1:

		//spawn the cube crumbling wall and set its transforms
		ACubeCrumbling* CWREF = GetWorld()->SpawnActor<ACubeCrumbling>(GetActorLocation(), FRotator(0, 0, 0), SpawnInfo);
		CWREF->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
		CWREF->setupCrumbleWall(crumbleSize);
		CWREF->AddActorWorldOffset(this->GetActorForwardVector() * 50 * GetActorScale3D().X);
		CWREF->SetActorScale3D(this->GetActorScale3D());
		CWREF->SetActorRotation(GetActorUpVector().Rotation());
		
		//fill the box with desired ore vein
		myOreSTRING = oreType;
		for (int i = 0; i < oreInside; i++) {

			FVector dispV = FVector(FMath::FRandRange(-20.0f, 20.0f), FMath::FRandRange(-20.0f, 20.0f), FMath::FRandRange(-20.0f, 20.0f));
			AMineableStone* stoneRef_ = GetWorld()->SpawnActor<AMineableStone>(GetActorLocation() + dispV, FRotator(0, FMath::FRandRange(-180.0f, 180.0f), 0), SpawnInfo);

			stoneRef_->SetActorScale3D(FVector(0.04f, 0.04f, 0.04f));
			stoneRef_->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
			stoneRef_->setVein(oreType);
			stoneRef_->StaticMeshComponent->SetScalarParameterValueOnMaterials(TEXT("IsStenciled"), 1.0f);
			stoneRef_->SetActorLocation(GetActorLocation() + dispV);

		}
		break;

	}
}
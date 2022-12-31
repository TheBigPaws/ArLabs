// Fill out your copyright notice in the Description page of Project Settings.


#include "ButterflyBB.h"

// Sets default values
AButterflyBB::AButterflyBB()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent_b = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent_b"));
	SetRootComponent(SceneComponent_b);


	StaticMeshComponent_b = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent_b"));
	StaticMeshComponent_b->SetupAttachment(SceneComponent_b);

	auto planeAsset = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Engine/BasicShapes/Plane.Plane'"));
	UStaticMesh* tempM = planeAsset.Object;
	StaticMeshComponent_b->SetStaticMesh(tempM);

	static ConstructorHelpers::FObjectFinder<UMaterial> FoundMaterial_door(TEXT("Material'/Game/GreenwoodFantasyVillage/Materials/MasterMaterials/M_SpriteAnim.M_SpriteAnim'"));
	if (FoundMaterial_door.Succeeded())
	{
		mat = FoundMaterial_door.Object;
	}
	mat_INST = UMaterialInstanceDynamic::Create(mat, NULL);
	StaticMeshComponent_b->SetMaterial(0, mat_INST);

	//set relative rotation of the component so that billboarding works
	StaticMeshComponent_b->SetRelativeRotation(FRotator(0,90,90));

	//create array of colour choices
	choices_.Add(FColor::Blue);
	choices_.Add(FColor::Red);
	choices_.Add(FColor::Green);
	choices_.Add(FColor::Cyan);
	choices_.Add(FColor::Magenta);
	choices_.Add(FColor::Emerald);
	choices_.Add(FColor::Orange);
	choices_.Add(FColor::Purple);
	choices_.Add(FColor::Turquoise);
	choices_.Add(FColor::White);
	choices_.Add(FColor::Yellow);
	int randIndex = FMath::RandRange(0, choices_.Num() - 1);
	FVector chosenColour = FVector(float(choices_[randIndex].R) / 255.0f, float(choices_[randIndex].G) / 255.0f, float(choices_[randIndex].B) / 255.0f);

	StaticMeshComponent_b->SetVectorParameterValueOnMaterials(TEXT("ButterflyColor"), chosenColour);
}

// Called when the game starts or when spawned
void AButterflyBB::BeginPlay()
{
	Super::BeginPlay();

	SetActorScale3D(FVector(0.1f, 0.1f, 0.1f));
	

}

// Called every frame
void AButterflyBB::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FVector dir = FVector(0,0,0);
	float dirSIZE = 0.0f;

	//if the camera pointer is valid, billboard rotate the butterfly
	if (camRef) {
		dir = camRef->GetComponentLocation() - GetActorLocation();
		dirSIZE = dir.Size();
	
		FRotator rot_ = dir.GetSafeNormal().Rotation();
		SetActorRotation(rot_);
	}

	//simple behaviour: follow if player is close, until they're far in which case idly fly about in random directions
	if (flyState == FString("Idle")) {
		AddActorWorldOffset(flyDir.GetSafeNormal() * DeltaTime * flySpeed);

		idleDirTimeLeft -= DeltaTime;
		if (idleDirTimeLeft <= 0.0f) {
			flyDir.X = FMath::FRandRange(-1.0f, 1.0f);
			flyDir.Y = FMath::FRandRange(-1.0f, 1.0f);
			flyDir.Z = FMath::FRandRange(-1.0f, 1.0f);
			idleDirTimeLeft = FMath::FRandRange(1.0f, 3.0f);
		}

		if (camRef && dirSIZE < START_FOLLOWING_TH) {
			flyState = FString("Following");
		}
	}
	else if (flyState == FString("Following") && camRef) {

		if (dirSIZE > CLOSE_STOP_TH) {
			AddActorWorldOffset(dir.GetSafeNormal() * DeltaTime * flySpeed);
		}
		if (dirSIZE > START_FOLLOWING_TH) {
			flyDir.X = FMath::FRandRange(-1.0f, 1.0f);
			flyDir.Y = FMath::FRandRange(-1.0f, 1.0f);
			flyDir.Z = FMath::FRandRange(-1.0f, 1.0f);
			idleDirTimeLeft = FMath::FRandRange(1.0f, 3.0f);
			flyState = FString("Idle");
		}
	}
	

}


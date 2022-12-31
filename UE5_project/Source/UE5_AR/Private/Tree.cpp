// Fill out your copyright notice in the Description page of Project Settings.


#include "Tree.h"



// Sets default values
ATree::ATree()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//set a random mesh out of a choice of 4
	auto MeshAssetA = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/GreenwoodFantasyVillage/Meshes/SM_Tree_A.SM_Tree_A'"));
	auto MeshAssetB = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/GreenwoodFantasyVillage/Meshes/SM_Tree_B.SM_Tree_B'"));
	auto MeshAssetC = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/GreenwoodFantasyVillage/Meshes/SM_Tree_C.SM_Tree_C'"));
	auto MeshAssetD = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/GreenwoodFantasyVillage/Meshes/SM_Tree_D.SM_Tree_D'"));
	UStaticMesh* tempM = nullptr;
	int choiceNR = FMath::RandRange(0, 3);


	if (choiceNR == 0 && MeshAssetA.Succeeded()) {
		tempM = MeshAssetA.Object;
	}
	else if (choiceNR == 1 && MeshAssetB.Succeeded()) {
		tempM = MeshAssetB.Object;
	}
	else if (choiceNR == 2 && MeshAssetC.Succeeded()) {
		tempM = MeshAssetC.Object;
	}
	else if (choiceNR == 3 && MeshAssetD.Succeeded()) {
		tempM = MeshAssetD.Object;
	}
	StaticMeshComponent->SetStaticMesh(tempM);
	



	//decide what Tier this tree will be (affecting its health, size and amount of logs it drops
	int randomSize = FMath::RandRange(0, 100);
	FVector buildScale = FVector(0, 0, 0);
	if (randomSize <= 50) { //50% for tier 1
		buildScale = FVector(0.15f, 0.15f, 0.4f);
		RSShealth = 10;
		logsInTree = 3;
	}
	else if (randomSize <= 80) { //30% for tier 2

		RSShealth = 50;
		logsInTree = 10;
		buildScale = FVector(0.3f, 0.3f, 0.9f);
	}
	else if (randomSize <= 20) { //20% for tier 3
		buildScale = FVector(0.8f, 0.8f, 1.5f);
		logsInTree = 50;
		RSShealth = 300;
	}

	//setup
	stateIncrementor = 0.0f;
	SetActorScale3D(buildScale);
	shakingDesiredRPY = FVector(0, 0, 0);
}

// Called when the game starts or when spawned
void ATree::BeginPlay()
{
	Super::BeginPlay();

	FVector3d boxExtent = GetComponentsBoundingBox().GetExtent();

	//calculte how far in front of the tree health text will be deisplaued
	textDispDist = sqrt(boxExtent.X * boxExtent.X + boxExtent.Y * boxExtent.Y) + 10.0f;

}

// Called every frame
void ATree::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	stateIncrementor += DeltaTime;

	//if text is referenced (meaning if it was created by the player hitting the tree for the first time), move the billboardd text in front of the tree ( from the player's perspective)
	if (healthTextRef) {
		healthTextRef->SetActorLocation(GetActorLocation() + FVector(0, 0, GetComponentsBoundingBox().GetExtent().Z / 3.0f) + (healthTextRef->camRef->GetComponentLocation() - GetActorLocation()).GetSafeNormal() * textDispDist);
	}

	//state based behaviour
	if (state_ == FString("Falling")) {
		if (stateIncrementor <= 1.0f) { // if the tree is still falling, update its rotation
			SetActorRotation(FRotator(stateIncrementor *90.0f,0,0));
		}
		if (stateIncrementor > 1.5f) { // if it laid on the ground for 0.5 extra seconds after finishing falling, delete it and spawn the logs

			//while there are logs to spawn
			while(logsInTree > 0)
			{

				//scatter the logs
				FVector dispV = FVector(FMath::FRandRange(-60.0f, 60.0f), FMath::FRandRange(-60.0f, 60.0f), 0);
				const FActorSpawnParameters SpawnInfo;

				//if there are 14 or more logs to spawn still, spawn those as a pile (to avoid having 14 individual logs)
				if (logsInTree >= 14) {
					ALogPile* LogPILERef_ = GetWorld()->SpawnActor<ALogPile>(GetActorLocation() + dispV, FRotator(0, FMath::FRandRange(-180.0f, 180.0f), 0), SpawnInfo);
					LogPILERef_->SetActorScale3D(FVector(0.4f, 0.4f, 0.4f));
					logsInTree -= 14;
				}
				else {
					ALogPickup* LogRef_ = GetWorld()->SpawnActor<ALogPickup>(GetActorLocation() + dispV, FRotator(0, FMath::FRandRange(-180.0f, 180.0f), 0), SpawnInfo);
					LogRef_->SetActorScale3D(FVector(0.4f, 0.4f, 0.4f));
					logsInTree--;
				}
				
			}
			//destroy text and itself
			healthTextRef->Destroy();
			healthTextRef = nullptr;
			this->Destroy();
		}
	}else if(state_ == FString("ShakingOUT")) //if the tree has just been hit. it will shake ooutwards for a little bit
	{
		SetActorRotation(FRotator(stateIncrementor*20*shakingDesiredRPY.X, stateIncrementor * 5 * shakingDesiredRPY.Y, stateIncrementor * 5 * shakingDesiredRPY.Z));
		if (stateIncrementor >= 0.05f) {
			state_ = FString("ShakingIN");
			stateIncrementor = 0.0f;
		}
	}
	else if (state_ == FString("ShakingIN")) //and then shake back into straight position
	{
		SetActorRotation(FRotator((1-stateIncrementor * 10.0f) * shakingDesiredRPY.X, (1 - stateIncrementor * 2.5f) * shakingDesiredRPY.Y, (1 - stateIncrementor * 2.5f) * shakingDesiredRPY.Z));
		if (stateIncrementor >= 0.1f) {
			state_ = FString("Idle");
			stateIncrementor = 0.0f;
			SetActorRotation(FRotator(0, 0, 0));

		}
	}
	
}


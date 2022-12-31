// Fill out your copyright notice in the Description page of Project Settings.


#include "MineableStone.h"



// Sets default values
AMineableStone::AMineableStone()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	//choose a random mesh out of 4 for the ore
	auto MeshAssetA = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/GreenwoodFantasyVillage/Meshes/SM_RockA.SM_RockA'"));
	auto MeshAssetB = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/GreenwoodFantasyVillage/Meshes/SM_RockB.SM_RockB'"));
	auto MeshAssetC = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/GreenwoodFantasyVillage/Meshes/SM_RockC.SM_RockC'"));
	auto MeshAssetD = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/GreenwoodFantasyVillage/Meshes/SM_RockD.SM_RockD'"));
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

		
	////create random colour on material
	//auto Mat_o = StaticMeshComponent->GetMaterial(0);
	//
	//UMaterialInstanceDynamic* myMat = UMaterialInstanceDynamic::Create(Mat_o, NULL);
	//StaticMeshComponent->SetMaterial(0, myMat);
	//
	//FVector4 randCol = FVector4(0.0f, 0.0f, 1.0f, 1.0f);
	//randCol.X = FMath::FRandRange(0.0f, 1.0f);
	//randCol.Y = FMath::FRandRange(0.0f, 1.0f);
	//randCol.Z = FMath::FRandRange(0.0f, 1.0f);
	//
	//myMat->SetVectorParameterValue(TEXT("OreColour"), randCol);
	////myMat->
	


}

// Called when the game starts or when spawned
void AMineableStone::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMineableStone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//set the desired ore vein
void  AMineableStone::setVein(FString veinType) {

	FVector4 oreCLR = FVector4(1.0f, 1.0f, 1.0f, 1.0f);


	if (veinType == FString("Stone")) {
		oreVein = veinType;

	}else if (veinType == FString("Iron")) {
		oreVein = veinType;
		oreCLR = FVector4(0.4f, 0.3f, 0.3f, 1.0f);
		StaticMeshComponent->SetVectorParameterValueOnMaterials(TEXT("OreColour"), oreCLR);
		StaticMeshComponent->SetScalarParameterValueOnMaterials(TEXT("HasOre"), 1.0f);
	}
	else if (veinType == FString("Mithril")) {
		oreVein = veinType;
		oreCLR = FVector4(0.0f, 0.3f, 1.0f, 1.0f);
		StaticMeshComponent->SetVectorParameterValueOnMaterials(TEXT("OreColour"), oreCLR);
		StaticMeshComponent->SetScalarParameterValueOnMaterials(TEXT("HasOre"), 1.0f);
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("probably badly specified name of vein"));
	}
}

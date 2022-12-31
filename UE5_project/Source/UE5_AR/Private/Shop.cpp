// Fill out your copyright notice in the Description page of Project Settings.


#include "Shop.h"
#include "Kismet/GameplayStatics.h"



// Sets default values
AShop::AShop()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComponent);
	
	
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(SceneComponent);


	UStaticMesh* tempM = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/GreenwoodFantasyVillage/Meshes/SM_ShopC.SM_ShopC'")).Object;
	StaticMeshComponent->SetStaticMesh(tempM);
	


}

// Called when the game starts or when spawned
void AShop::BeginPlay()
{
	Super::BeginPlay();
	


}

// Called every frame
void AShop::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//tier up upgrade costs
void AShop::TierUp() {
	upgradeTier++;

	switch (upgradeTier) {
	case 2:
		Wood_COST = 30;
		Stone_COST = 5;
		Iron_COST = 10;
		break;
	case 3:
		Wood_COST = 50;
		Stone_COST = 0;
		Iron_COST = 20;
		Mithril_COST = 10;
		break;
	case 4:
		Wood_COST = 100;
		Stone_COST = 0;
		Iron_COST = 30;
		Mithril_COST = 30;
		break;
	default:
		break;
	}
}

//check if given the player materials, one is able to upgrade their tool
bool AShop::CanUpgradeTool(int playerWood, int playeStone, int playerIron, int playerMithril) {


	if (Wood_COST <= playerWood && Stone_COST <= playeStone && Iron_COST <= playerIron && Mithril_COST <= playerMithril) {
		return true;
	}

	return false;
}

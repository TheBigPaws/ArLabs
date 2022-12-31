// Fill out your copyright notice in the Description page of Project Settings.


#include "LogPile.h"

// Fill out your copyright notice in the Description page of Project Settings.


// Sets default values
ALogPile::ALogPile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	UStaticMesh* tempM = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/GreenwoodFantasyVillage/Meshes/InventoryProps/SM_Logs.SM_Logs'")).Object;
	StaticMeshComponent->SetStaticMesh(tempM);

}

// Called when the game starts or when spawned
void ALogPile::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ALogPile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	

}


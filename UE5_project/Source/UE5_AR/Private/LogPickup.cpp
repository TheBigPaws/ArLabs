// Fill out your copyright notice in the Description page of Project Settings.


#include "LogPickup.h"

// Sets default values
ALogPickup::ALogPickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	UStaticMesh* tempM = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/GreenwoodFantasyVillage/Meshes/InventoryProps/SM_Log.SM_Log'")).Object;
	StaticMeshComponent->SetStaticMesh(tempM);
}

// Called when the game starts or when spawned
void ALogPickup::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALogPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


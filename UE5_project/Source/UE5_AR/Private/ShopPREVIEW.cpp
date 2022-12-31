// Fill out your copyright notice in the Description page of Project Settings.


#include "ShopPREVIEW.h"

// Sets default values
AShopPREVIEW::AShopPREVIEW()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComponent);


	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(SceneComponent);



	UStaticMesh* tempM = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/GreenwoodFantasyVillage/Meshes/SM_ShopC.SM_ShopC'")).Object;
	StaticMeshComponent->SetStaticMesh(tempM);

	static ConstructorHelpers::FObjectFinder<UMaterial>FoundMaterial(TEXT("Material'/Game/Assets/Materials/greenMat.greenMat'"));
	if (FoundMaterial.Succeeded())
	{
		StoredMaterial = FoundMaterial.Object;
	}
	OnMat = UMaterialInstanceDynamic::Create(StoredMaterial, StaticMeshComponent);
	StaticMeshComponent->SetMaterial(0, OnMat);

}

// Called when the game starts or when spawned
void AShopPREVIEW::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AShopPREVIEW::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


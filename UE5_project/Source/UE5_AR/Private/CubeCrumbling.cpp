// Fill out your copyright notice in the Description page of Project Settings.


#include "CubeCrumbling.h"

#include "Kismet/GameplayStatics.h"
#include <string>
#include "Components/PrimitiveComponent.h"
#include "CustomARPawn.h"
#include "GameFramework/DefaultPawn.h"
#include "Engine/EngineTypes.h"

// Sets default values
ACubeCrumbling::ACubeCrumbling()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComponent);


}

// Called when the game starts or when spawned
void ACubeCrumbling::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACubeCrumbling::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (crumblePieceArray.Num() > 0) {
		for (int i = 0; i < crumblePieceArray.Num(); i++) {
			crumblePieceArray[i]->updateMe(DeltaTime);
			if (crumblePieceArray[i]->lifeTime <= 0.0f) {
				crumblePieceArray[i]->Destroy();
				crumblePieceArray.RemoveAt(i);
				break;
			}

		}
	}

	

}

//this gets called when the player presses on a piece of the crumbling wall. 
void ACubeCrumbling::partlyCrumble(int miningStrength) {

	//destroy as many pieces as strong the player's tool is
	for (int i = 0; i < miningStrength; i++) {

		//if there are no more solid pieces stop
		if (solidPieceArray.Num() == 0) {
			return;
		}
		else {
			
			//choose a random piece of the wall to fall off
			int pos_;
			pos_ = FMath::RandRange(0, solidPieceArray.Num() - 1);

			//add this piece to the array of references to alive pieces
			crumblePieceArray.Add(solidPieceArray[pos_]);

			//set up this previously inactive actor to be dynamic, using tick and simulating physics
			float lifeTime_ = FMath::FRandRange(0.9f, 1.4f);
			crumblePieceArray.Last()->SetActorTickEnabled(true);
			crumblePieceArray.Last()->scaleDecrementor = crumblePieceArray.Last()->cubeMeshComponent->GetComponentScale().X / 2.0f * lifeTime_;
			crumblePieceArray.Last()->lifeTime = lifeTime_;
			crumblePieceArray.Last()->cubeMeshComponent->SetSimulatePhysics(true);
			crumblePieceArray.Last()->cubeMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			crumblePieceArray.Last()->dynamic = true;

			//apply impulse to the newly ffallen off piece
			FVector ImpulseVector = this->GetActorUpVector();
			ImpulseVector *= -1;
			crumblePieceArray.Last()->cubeMeshComponent->SetMassOverrideInKg(NAME_None, 1.0f);
			FVector scale_ = crumblePieceArray.Last()->GetActorScale3D();
			crumblePieceArray.Last()->SetActorScale3D(scale_ * 0.8f);
			crumblePieceArray.Last()->cubeMeshComponent->AddImpulse((ImpulseVector.X * FVector(FMath::FRandRange(0.8f, 1.2f), ImpulseVector.Y * FMath::FRandRange(0.8f, 1.2f),0.2f)) * FMath::FRandRange(100.0f, 300.0f));


			//remove the reference to this piece being solid
			solidPieceArray.RemoveAt(pos_);
		}

		
	}

}


//setting up the crumbling wall, consisting of CrumbleWallPieces
void ACubeCrumbling::setupCrumbleWall(int sideDimension)
{

	crumbleWallSize = sideDimension;
	
	float cubeScale = 1.0f / crumbleWallSize;
	CCscale = cubeScale;

	//health (number of pieces) is alwas dimensions^2
	for (int x_ = 0; x_ < crumbleWallSize; x_++) {
		for (int y_ = 0; y_ < crumbleWallSize; y_++) {

			//calculate the iterated location
			FVector MyLoc = FVector(-50 + cubeScale * 100.0f * x_ + cubeScale*50, -50 + cubeScale * y_ * 100.0f + cubeScale * 50,0.0f);

			const FActorSpawnParameters SpawnInfo;
			const FRotator MyRot(0, 0, 0);

			//set up the crumble piece
			ACreumbleWallPiece* ref_ = GetWorld()->SpawnActor<ACreumbleWallPiece>(SceneComponent->GetComponentLocation(), MyRot, SpawnInfo);
			ref_->SetActorScale3D(FVector(CCscale, CCscale, 0.1f));
			ref_->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
			ref_->AddActorLocalOffset(MyLoc);
			ref_->parentRef = this;
			solidPieceArray.Add(ref_);
			
		}
	}

	

}

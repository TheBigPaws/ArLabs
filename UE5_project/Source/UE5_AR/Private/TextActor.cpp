// Fill out your copyright notice in the Description page of Project Settings.


#include "TextActor.h"

// Sets default values
ATextActor::ATextActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	myTextComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("BillboardedTXT"));
	myTextComponent->SetHorizontalAlignment(EHTA_Center);
	myTextComponent->SetVerticalAlignment(EVRTA_TextBottom);

}

// Called when the game starts or when spawned
void ATextActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATextActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//BB

	//add movement to the text
	AddActorWorldOffset(velocity * DeltaTime);
	lifeLeft -= DeltaTime;

	//if it is timeDestroyable (e.g. tree health is not), see if its life time has ran out yet
	if (lifeLeft <= 0.0f && timeDestroyable) {
		this->Destroy();
	}

	//if the camera reference is linked up, billboard towards it.
	if (camRef) {
		FVector dir = camRef->GetComponentLocation() - GetActorLocation();
		dir.Normalize();
		FRotator rot_ = dir.Rotation();
		//rot_.Pitch /= 3.0f;
		myTextComponent->SetRelativeRotation(rot_);
	}

}

//setup function for ease of use
void ATextActor::setupText(FString dispText, float howLongToDisplay, FColor color, float size, UCameraComponent* PawnCamRef, FVector moveVelocity) {

	myString = dispText;
	lifeLeft = howLongToDisplay;
	velocity = moveVelocity;

	camRef = PawnCamRef;
	myTextComponent->SetTextRenderColor(color);
	
	myTextComponent->SetWorldSize(size);


	myTextComponent->SetText(FText::FromString(dispText));
}

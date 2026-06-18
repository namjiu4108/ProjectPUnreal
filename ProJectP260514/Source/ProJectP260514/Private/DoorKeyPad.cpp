// Fill out your copyright notice in the Description page of Project Settings.


#include "DoorKeyPad.h"

// Sets default values
ADoorKeyPad::ADoorKeyPad()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

void ADoorKeyPad::BeginPlay()
{
	Super::BeginPlay();
}

void ADoorKeyPad::OpenDoor()
{
	// 키패드의 매쉬를 찾음
	UStaticMeshComponent* FoundMesh = FindComponentByClass<UStaticMeshComponent>();

	if (FoundMesh)
	{
		// 머티리얼 변경
		if (UnlockMat0)
		{
			FoundMesh->SetMaterial(0, UnlockMat0);
		}
		if (UnlockMat2)
		{
			FoundMesh->SetMaterial(2, UnlockMat2);
		}
	}

	// 블루프린트로 신호를 보내 시각적인 문 열림 신호 발생
	OnDoorOpened();
}

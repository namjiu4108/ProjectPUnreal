// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DoorKeyPad.generated.h"

UCLASS()
class PROJECTP260514_API ADoorKeyPad : public AActor
{
	GENERATED_BODY()
public:
	// Sets default values for this actor's properties
	ADoorKeyPad();

protected:
	virtual void BeginPlay() override;

	// 키패드 unlock시 머티리얼 변경
	// unlock 시 적용할 머티리얼
	UPROPERTY(EditAnywhere, Category = "Hacking")
	UMaterialInterface* UnlockMat0;

	UPROPERTY(EditAnywhere, Category = "Hacking")
	UMaterialInterface* UnlockMat2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hacking")
	TArray<AActor*> TargetDoors;

public:
	// 해킹이 성공 시 호출될 함수
	UFUNCTION(BlueprintCallable, Category = "Hacking")
	void OpenDoor();

	// C++에서 해킹 성공을 알리면, 블루프린트에서 문이 열리는 애니메이션(타임라인 등)을 재생할 이벤트
	UFUNCTION(BlueprintImplementableEvent, Category = "Hacking")
	void OnDoorOpened();
};

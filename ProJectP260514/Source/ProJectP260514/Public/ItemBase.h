// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ST_ItemData.h"
#include "ItemBase.generated.h"

UCLASS()
class PROJECTP260514_API AItemBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// 이 함수는 C++에서 선언만 하지만, 구현은 블루프린트에서
	UFUNCTION(BlueprintImplementableEvent, Category = "Item")
	void InitItem(const FItemData& Data, FDataTableRowHandle Handle);
};

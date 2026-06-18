// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_DetectPlayer.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTP260514_API UBTService_DetectPlayer : public UBTService_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UBTService_DetectPlayer();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

public:
	// 총 감지 거리 (플레이어를 발견하는 범위)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Distance")
	float DetectRadius = 1000.0f; 

	// 이 거리 안으로 들어오면 속도를 줄여서 걸어옴
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Distance")
	float WalkRadius = 400.0f;

	// 뛸 때 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Speed")
	float RunSpeed = 600.0f;

	// 걸을 때 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Speed")
	float WalkSpeed = 300.0f;
};

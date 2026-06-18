// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_DianaFollow.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTP260514_API UBTService_DianaFollow : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_DianaFollow();

protected:
	// 비헤이비어 트리에서 주기적으로 실행될 로직
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	// 블루프린트에서 키를 선택할 수 있게 변수 추가
	UPROPERTY(EditAnywhere, Category = "AI")
	struct FBlackboardKeySelector BlackboardKey;
	
};

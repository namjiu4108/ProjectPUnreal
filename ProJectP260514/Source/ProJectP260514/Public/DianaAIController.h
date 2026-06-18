// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "DianaAIController.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTP260514_API ADianaAIController : public AAIController
{
	GENERATED_BODY()

public:
	ADianaAIController();

	// 비헤이비어 트리를 담을 변수 선언
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	class UBehaviorTree* BehaviorTreeAsset;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override; // 추가
};

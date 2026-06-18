// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_EnemyAttack.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTP260514_API UBTTask_EnemyAttack : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UBTTask_EnemyAttack();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};

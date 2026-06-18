// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_EnemyAttack.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "Enemy.h"

UBTTask_EnemyAttack::UBTTask_EnemyAttack()
{
	NodeName = TEXT("Enemy Attack");
}

EBTNodeResult::Type UBTTask_EnemyAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (AIController == nullptr) return EBTNodeResult::Failed;

	// AEnemy로 형변환(Cast)
	AEnemy* Enemy = Cast<AEnemy>(AIController->GetPawn());
	if (Enemy == nullptr) return EBTNodeResult::Failed;

	// 몬스터에게 공격하라고 명령 내리기!
	Enemy->EnemyAttack();

	// 공격 애니메이션을 실행했으니 태스크 성공 반환
	return EBTNodeResult::Succeeded;
}

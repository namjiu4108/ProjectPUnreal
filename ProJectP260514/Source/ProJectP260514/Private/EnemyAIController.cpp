// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"

AEnemyAIController::AEnemyAIController()
{
}

void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (BehaviorTreeAsset)
	{
		// 1. 비헤이비어 트리 실행
		RunBehaviorTree(BehaviorTreeAsset);

		// 2. 블랙보드가 정상 작동 중이라면, 몬스터의 처음 위치(HomeLocation)를 저장
		if (Blackboard)
		{
			Blackboard->SetValueAsVector(TEXT("HomeLocation"), InPawn->GetActorLocation());
		}
	}
}

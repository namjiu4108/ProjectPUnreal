// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_DianaFollow.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"          // ◀ 이 헤더를 추가해야 GetAIOwner()를 인식합니다.
#include "GameFramework/Pawn.h"   // ◀ 이 헤더를 추가해야 GetPawn()을 인식합니다.

UBTService_DianaFollow::UBTService_DianaFollow()
{
	NodeName = "Find Player Location"; // 에디터에서 볼 이름
}

void UBTService_DianaFollow::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    // 1. 플레이어 폰 가져오기
    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

    // 2. 블랙보드에 플레이어 위치 저장
    if (PlayerPawn)
    {
        // 1. 위치 업데이트 (기존 로직)
        OwnerComp.GetBlackboardComponent()->SetValueAsVector(TEXT("TargetLocation"), PlayerPawn->GetActorLocation());

        // 2. 거리 계산 및 블랙보드 저장 (수정 로직)
        AAIController* AIController = OwnerComp.GetAIOwner();
        if (AIController && AIController->GetPawn())
        {
            float Distance = FVector::Dist(AIController->GetPawn()->GetActorLocation(), PlayerPawn->GetActorLocation());
            OwnerComp.GetBlackboardComponent()->SetValueAsFloat(TEXT("DistanceToPlayer"), Distance);
        }

        // 3. 플레이어라는 존재 자체를 블랙보드에 저장
        OwnerComp.GetBlackboardComponent()->SetValueAsObject(TEXT("TargetActor"), PlayerPawn);
    }
}

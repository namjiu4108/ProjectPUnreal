// Fill out your copyright notice in the Description page of Project Settings.


#include "DianaAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

ADianaAIController::ADianaAIController()
{
    // 컨트롤러가 포커스를 맞출 때 캐릭터를 회전시키도록 설정
    bSetControlRotationFromPawnOrientation = false;
}

void ADianaAIController::BeginPlay()
{
    Super::BeginPlay();

    // 에디터에서 설정한 BehaviorTree가 있다면 실행
    if (BehaviorTreeAsset != nullptr)
    {
        RunBehaviorTree(BehaviorTreeAsset);
    }
}

void ADianaAIController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    //// 플레이어를 가져와서 포커스 설정
    //APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    //if (PlayerPawn)
    //{
    //    SetFocus(PlayerPawn);
    //}
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_DetectPlayer.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h" // 속도 제어 위한 #include

UBTService_DetectPlayer::UBTService_DetectPlayer()
{
	NodeName = TEXT("Detect Player");

	// 매 프레임 검사하면 무거우니 0.4초마다 한 번씩 거리 계산하도록 간격 설정
	Interval = 0.4f;
	RandomDeviation = 0.1f;
}

void UBTService_DetectPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	// AI 컨트롤러와 몬스터 폰 가져오기
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (AIController == nullptr) return;

	// CharacterMovement를 쓰기 위해 APawn 대신 ACharacter로 형변환
	ACharacter* EnemyChar = Cast<ACharacter>(AIController->GetPawn());
	if (EnemyChar == nullptr) return;

	UCharacterMovementComponent* MoveComp = EnemyChar->GetCharacterMovement();
	if (MoveComp == nullptr) return;

	// 플레이어 캐릭터 가져오기
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (PlayerCharacter == nullptr) return;

	// 몬스터와 플레이어 사이의 거리 계산
	float Distance = FVector::Distance(EnemyChar->GetActorLocation(), PlayerCharacter->GetActorLocation());

	// 블랙보드 가져오기
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (BlackboardComp == nullptr) return;

	if (Distance <= DetectRadius)
	{
		// 1. 감지 거리 안으로 들어오면 플레이어를 타겟으로 설정
		// GetSelectedBlackboardKey()는 우리가 에디터에서 지정할 변수(TargetActor)를 의미함
		BlackboardComp->SetValueAsObject(GetSelectedBlackboardKey(), PlayerCharacter);

		// 2. 여기서 거리에 따라 속도를 동적으로 분기!
		if (Distance <= WalkRadius)
		{
			// 플레이어와 가까워지면(400 이하) 슬슬 걸어옴 (Walk 애니메이션 출력)
			MoveComp->MaxWalkSpeed = WalkSpeed;
		}
		else
		{
			// 아직 멀리 있을 때는 맹렬하게 뛰어옴 (Run 애니메이션 출력)
			MoveComp->MaxWalkSpeed = RunSpeed;
		}
	}
	else
	{
		// 플레이어를 놓쳤을 때
		BlackboardComp->ClearValue(GetSelectedBlackboardKey());

		// 자기 자리로 복귀할 때는 터덜터덜 걸어서 돌아가게 설정
		MoveComp->MaxWalkSpeed = WalkSpeed;
	}
}

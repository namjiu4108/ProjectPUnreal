// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "PlayerHue.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "AIController.h"
#include "BrainComponent.h"
#include "Engine//DataTable.h"
#include "ItemBase.h"
#include "Components/WidgetComponent.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 기본 스탯 초기화
	MaxHP = 100.0f;
	Defense = 5.0f;  // 예시: 방어력 5
	bIsDead = false;

	// 스폰되거나 맵에 배치될 때 자동으로 AI가 조종하도록 설정
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// 생성자에서 위젯 컴포넌트 생성 및 메시에 부착
	HPBarComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBarComponent"));
	HPBarComponent->SetupAttachment(GetMesh()); 

	// 컴포넌트 위치 조정 (적 머리 위 적당한 높이로)
	HPBarComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 210.0f));
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	// 시작할 때 현재 체력을 최대 체력으로 설정
	CurrentHP = MaxHP;
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	// 기본 TakeDamage 실행
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// 1. 이미 죽었다면 데미지 무시
	if (bIsDead || ActualDamage <= 0.0f)
	{
		return 0.0f;
	}

	// 2. 방어력 계산 (공식: 받은 데미지 - 방어력)
	// 방어력이 데미지보다 높아서 음수가 나오면 0으로 처리 (피가 차는 것을 방지)
	float FinalDamage = FMath::Max(0.0f, ActualDamage - Defense);

	// 3. 현재 체력 감소
	CurrentHP -= FinalDamage;

	// 로그로 데미지 계산 확인
	UE_LOG(LogTemp, Warning, TEXT("Enemy Took Damage! Original: %f, Defense: %f, Final: %f, Current HP: %f"), ActualDamage, Defense, FinalDamage, CurrentHP);

	// 죽지 않았음 && 체력이 최대 체력의 절반(50%) 이하임 && 아직 이 애니메이션을 안 틀었음
	if (CurrentHP > 0.0f && CurrentHP <= (MaxHP * 0.5f) && !PlayHalfHPHitAnim)
	{
		// 한번 이 조건이 실행되면 조건이 되지않게함
		PlayHalfHPHitAnim = true;

		// 피격 애니메이션 실행
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

		// 안전검사
		if (AnimInstance && HitMontage)
		{
			AnimInstance->Montage_Play(HitMontage);
			UE_LOG(LogTemp, Warning, TEXT("Enemy HP dropped below 50%%! Playing Hit Stun."));

			// AI정지
			AAIController* AIController = Cast<AAIController>(GetController());
			if (AIController)
			{
				// 걷기/ 뛰기 명령 정지
				AIController->StopMovement();

				// 비헤이비어 트리 종료
				if (AIController->GetBrainComponent())
				{
					// "HalfHPStun" 이라는 이름으로 로직 정지
					AIController->GetBrainComponent()->PauseLogic("HalfHPStun");
				}
			}

			// 경직 해제 타이머 몽타주 재생 길이만큼 멈췄다가 다시 움직임
			float StunDuration = HitMontage->GetPlayLength();
			GetWorldTimerManager().SetTimer(HitStunTimerHandle, this, &AEnemy::RecoverFromHitStun, StunDuration, false);
		}
	}

	// 4. 체력이 0 이하가 되면 사망 처리
	if (CurrentHP <= 0.0f)
	{
		Die();
	}

	return FinalDamage;
}

void AEnemy::RecoverFromHitStun()
{
	UE_LOG(LogTemp, Warning, TEXT("RecoverFromHitStun Called! Defense: %f"), Defense);
	// 경직 중에 죽었다면 그냥 리턴
	if (bIsDead) return;

	// AI 비헤이비어 트리 다시 재생
	AAIController* AIController = Cast<AAIController>(GetController());
	if (AIController && AIController->GetBrainComponent())
	{
		// "HalfHPStun" 이라는 이름으로 로직 정지
		AIController->GetBrainComponent()->ResumeLogic("HalfHPStun");
	}
	UE_LOG(LogTemp, Warning, TEXT("Enemy recovered from Half HP Stun!"));
}

void AEnemy::Die()
{
	// 이미 죽었다면 중복 실행 방지
	if (bIsDead) return;

	bIsDead = true;

	// 월드에서 다이애나(ADiana) 객체를 직접 찾아서 경험치 전달
	AActor* FoundDiana = UGameplayStatics::GetActorOfClass(GetWorld(), ADiana::StaticClass());
	ADiana* DianaRef = Cast<ADiana>(FoundDiana);
	if (DianaRef)
	{
		DianaRef->AddEXP(DropEXP);
	}

	// 데이터 테이블과 아이템이 에디터에서 할당되었는지 확인
	if (ItemClass && ItemDataTable)
	{
		// 몬스터 위치에 아이템 스폰
		AItemBase* SpawnedItem = GetWorld()->SpawnActor<AItemBase>(ItemClass, GetActorLocation(), FRotator::ZeroRotator);

		if (SpawnedItem)
		{
			// 1. 블루프린트의 ItemRowName 변수에 넣을 '핸들'을 만듦
			FDataTableRowHandle NewHandle;
			NewHandle.DataTable = ItemDataTable;
			NewHandle.RowName = DropItemRowName;

			// 2. 데이터 테이블에서 실제 데이터를 찾음
			FItemData* Data = ItemDataTable->FindRow<FItemData>(DropItemRowName, TEXT("ItemDrop"));

			// 3. 데이터와 핸들을 모두 블루프린트로 넘김
			if (Data)
			{
				SpawnedItem->InitItem(*Data, NewHandle);
			}
		}
	}

	// 사망 애니메이션 실행
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	// 안전검사
	if (AnimInstance && DeadMontage)
	{
		AnimInstance->Montage_Play(DeadMontage);

		// 로그로 사망 출력 확인
		UE_LOG(LogTemp, Warning, TEXT("Enemy Dead!"));
	}

	// 콜리전 해제, 필드에 콜리전이 남아있지않게 하기
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// AI정지
	AAIController* AIController = Cast<AAIController>(GetController());
	if (AIController)
	{
		// 비헤이비어 트리 종료
		if (AIController->GetBrainComponent())
		{
			AIController->GetBrainComponent()->StopLogic("Enemy is Dead");
		}

		// 걷기/ 뛰기 명령 정지
		AIController->StopMovement();
	}
	// 시체 없애기(사망 애니메이션이 끝날때 즈음 삭제)
	SetLifeSpan(2.8f);
}

void AEnemy::EnemyAttack()
{
	// 이미 죽었거나, 에디터에서 공격 몽타주를 등록하지 않았다면 실행 안 함
	if (bIsDead || !AttackMontage) return;

	// 애니메이션 인스턴스를 가져와서 몽타주 재생
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Play(AttackMontage);

		// 로그로 공격 출력 확인
		UE_LOG(LogTemp, Warning, TEXT("Enemy Attacks Player!"));
	}
}

void AEnemy::EnemyAttackHitCheck()
{
	if (bIsDead) return;

	// [핵심] 언리얼 기본 마네킹의 머리 뼈 이름인 "head"의 실시간 위치를 가져옴
	// 애니메이션 때문에 머리가 앞으로 숙여지면, 이 좌표도 같이 앞으로 따라감 
	FVector HeadLocation = GetMesh()->GetSocketLocation(TEXT("head"));

	// 시작점(Start): 머리 위치에서 몬스터가 바라보는 방향으로 살짝(20cm) 앞
	FVector Start = HeadLocation + GetActorForwardVector() * 20.0f; 

	// 끝점(End): 시작점에서 에디터에서 설정한 사거리(AttackRange)만큼 전방으로 이동
	FVector End = Start + GetActorForwardVector() * AttackRange;

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this); // 몬스터 자신은 판정에서 제외

	// 전방 캡슐/구체 범위에 플레이어가 걸리는지 충돌 검사
	bool bHit = GetWorld()->SweepSingleByChannel(
		HitResult,
		Start,
		End,
		FQuat::Identity,
		ECC_Visibility,
		FCollisionShape::MakeSphere(AttackRadius),
		QueryParams
	);

	// [디버그용] 주석을 해제하면 에디터에서 공격 범위를 빨간 구체로 시각화
	DrawDebugSphere(GetWorld(), Start, AttackRadius, 12, FColor::Red, false, 1.0f);

	if (bHit)
	{
		// 맞은 대상이 플레이어(PlayerHue)인지 확인
		APlayerHue* Player = Cast<APlayerHue>(HitResult.GetActor());
		if (Player)
		{
			// 변수화한 AttackDamage를 사용하여 데미지 전달
			UGameplayStatics::ApplyDamage(Player, AttackDamage, GetController(), this, UDamageType::StaticClass());

			UE_LOG(LogTemp, Warning, TEXT("Enemy Hit Player Successfully!"));
		}
	}
}

void AEnemy::ApplyHackingDebuff()
{
	// 이미 해킹 디버프 타이머가 돌고 있는 상태가 아니라면
	if (!GetWorldTimerManager().IsTimerActive(DebuffTimerHandle))
	{
		OriginDefense = Defense; // 원래 방어력 할당
	}

	// 방어력을 0으로 만듦
	Defense = 0.0f;

	// 20초 뒤에 RestoreDefense 함수를 단 한 번(false) 실행하도록 타이머 설정
	GetWorldTimerManager().SetTimer(DebuffTimerHandle, this, &AEnemy::RestoreDefense, 20.0f, false);
}

void AEnemy::RestoreDefense()
{
	// 방어력 원래 방어력으로 복구
	Defense = OriginDefense; // 원래 방어력 할당
}

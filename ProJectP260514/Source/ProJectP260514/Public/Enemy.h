// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

UCLASS()
class PROJECTP260514_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// 해킹 디버프 관련 변수
	FTimerHandle DebuffTimerHandle; // 디버프 타이머(20초 설정)
	float OriginDefense; // 원래 방어력 기억할 변수

	// 체력이 절반 이하로 떨어져서 애니메이션을 재생
	bool PlayHalfHPHitAnim = false;

	// 타이머 핸들 (경직 해제용)
	FTimerHandle HitStunTimerHandle;
	void RecoverFromHitStun();

	// 에디터에서 드랍할 아이템 블루프린트 클래스
	UPROPERTY(EditAnywhere, Category = "Drop")
	TSubclassOf<class AItemBase> ItemClass;

	// 에디터에서 아이템 데이터 테이블을 지정하기 위한 클래스
	UPROPERTY(EditAnywhere, Category = "Drop")
	class UDataTable* ItemDataTable;

	// 드랍 아이템 이름
	UPROPERTY(EditAnywhere, Category = "Drop")
	FName DropItemRowName = FName("Item_Ammo");

	// 머리 위 UI를 담당할 위젯 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* HPBarComponent;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// === [스탯 관련 변수] ===

	// 최대 체력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float MaxHP;

	// 현재 체력
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Stat")
	float CurrentHP;

	// 방어력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float Defense;

	// 몬스터 공격 데미지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackDamage = 50.0f;

	// 몬스터 공격 사거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackRange = 100.0f;

	// 몬스터 공격 판정 구체 반지름
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackRadius = 60.0f;

	// 적 처치 시 다이애나에게 지급할 경험치 수치
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reward")
	float DropEXP = 50.0f;

	// 죽었는지 여부
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	bool bIsDead;

	// === [데미지 처리 함수] ===

	// 언리얼 엔진 기본 데미지 전달 함수 오버라이드
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	// 사망 처리 함수
	void Die();

	// 에디터에서 할당할 공격 애니메이션 몽타주
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class UAnimMontage* AttackMontage;

	// 에디터에서 할당할 Dead 애니메이션 몽타주
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class UAnimMontage* DeadMontage;

	// 에디터에서 할당할 Hit 애니메이션 몽타주
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	class UAnimMontage* HitMontage;

	// 공격 실행 함수
	void EnemyAttack();

	// 공격 판정 함수
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void EnemyAttackHitCheck();

	// 해킹 성공 시 플레이어가 호출해줄 함수
	void ApplyHackingDebuff();

	// 20초 뒤 타이머에 의해 자동으로 호출되어 방어력을 되돌릴 함수
	void RestoreDefense();
};

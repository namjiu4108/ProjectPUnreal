// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Engine/DataTable.h" // 데이터테이블 상속을 위해 필수 추가
#include "Diana.generated.h"

// 다이애나 클래스 바로 위에 대화 데이터 구조체를 정의
USTRUCT(BlueprintType)
struct FDialogueStruct : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	FText DialogueText;
};

UCLASS()
class PROJECTP260514_API ADiana : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ADiana();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// 다이애나의 최대 MP와 현재 MP
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float MaxMP = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float CurrentMP = 100.0f;

	// MP 부족 상태를 기억할 플래그 변수
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Stat")
	bool bIsShortOfMP = false;

	// 해킹이 가능한 상태인지 체크
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Hacking")
	bool bCanHack = true;

	// 해킹 쿨타임 시간 (15초)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hacking")
	float HackingCooldown = 15.0f;

	// --- [레벨 및 경험치 변수] ---
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Stat")
	int32 CurrentLevel = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Stat")
	float CurrentEXP = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float MaxEXP = 100.0f; // 레벨업에 필요한 경험치

	// 쿨타임을 잴 타이머 핸들
	FTimerHandle HackingTimerHandle;

	// 스킬 사용 시 MP를 깎는 함수
	UFUNCTION(BlueprintCallable, Category = "Skill")
	bool UseMP(float Amount);

	// C++이 아닌 블루프린트에서 대사 텍스트와 애니메이션을 짜기 위한 이벤트
	UFUNCTION(BlueprintImplementableEvent, Category = "Skill")
	void OnSkillActivated();

	// MP가 회복되었을 때 블루프린트에서 대사를 띄우기 위한 이벤트
	UFUNCTION(BlueprintImplementableEvent, Category = "Skill")
	void OnMPRecoveredReady(FName DialogueRowName);

	// 해킹 전용 대사를 띄우기 위한 이벤트
	UFUNCTION(BlueprintImplementableEvent, Category = "Hacking")
	void PlayHackingDialogue(FName DialogueRowName);

	// 해킹을 실행했을 때 쿨타임을 돌리는 함수
	UFUNCTION(BlueprintCallable, Category = "Hacking")
	void StartHackingCooldown();

	// 타이머가 끝나면 쿨타임을 초기화해줄 함수
	UFUNCTION(BlueprintCallable, Category = "Hacking")
	void ResetHackingCooldown();

	// 해킹 남은 쿨타임 시간을 리턴해줄 함수 추가
	UFUNCTION(BlueprintPure, Category = "Hacking")
	float GetHackingRemainingTime() const;

	// 외부(Enemy)에서 경험치를 넣어줄 함수
	UFUNCTION(BlueprintCallable, Category = "Stat")
	void AddEXP(float Amount);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	// 내부 레벨업 처리 함수
	void LevelUp();
};

// Fill out your copyright notice in the Description page of Project Settings.


#include "Diana.h"
#include "PragmataGameInstance.h"

// Sets default values
ADiana::ADiana()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ADiana::BeginPlay()
{
	Super::BeginPlay();
	// 게임 데이터 불러오기
	UPragmataGameInstance* GI = Cast<UPragmataGameInstance>(GetGameInstance());

	if (GI && GI->bHasSavedData)
	{
		CurrentLevel = GI->SavedDianaLevel;
		CurrentEXP = GI->SavedDianaExp;
		MaxEXP = GI->SavedDianaMaxExp;
		MaxMP = GI->SavedDianaMaxMp;
		CurrentMP = GI->SavedDianaCurrentMp;
	}
	else
	{
		// MP를 MaxMp로 채움
		CurrentMP = MaxMP;
	}
}

bool ADiana::UseMP(float Amount)
{
	if (CurrentMP >= Amount)
	{
		CurrentMP -= Amount;

		// 블루프린트로 "스킬 발동" 신호 보내기
		OnSkillActivated();
		return true;
	}
	UE_LOG(LogTemp, Warning, TEXT("Diana : MP no..."));
	return false;
}

// Called every frame
void ADiana::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 다이애나의 MP가 1초에 0.5 씩 회복 되도록 deltatime을 곱함
	// FMath::Clamp를 써서 MaxMp를 넘지 않도록 함
	if (CurrentMP < MaxMP)
	{
		CurrentMP = FMath::Clamp(CurrentMP + (0.5 * DeltaTime), 0.0f, MaxMP);
	}

	// MP 부족 상태였는데 30 이상으로 회복되었다면?
	if (bIsShortOfMP && CurrentMP >= 30.0f)
	{
		bIsShortOfMP = false; // 대사가 무한 반복되지 않도록 상태 해제
		OnMPRecoveredReady(FName("8")); // 블루프린트로 신호 발송
	}

}

// Called to bind functionality to input
void ADiana::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ADiana::StartHackingCooldown()
{
	// 해킹 쿨타임 시작
	bCanHack = false;
	GetWorldTimerManager().SetTimer(HackingTimerHandle, this, &ADiana::ResetHackingCooldown, HackingCooldown, false);

	PlayHackingDialogue(FName("10"));
	UE_LOG(LogTemp, Warning, TEXT("Diana: Hacking Cooldown Started."));
}
void ADiana::ResetHackingCooldown()
{
	// 해킹 쿨타임 종료
	bCanHack = true;
	UE_LOG(LogTemp, Warning, TEXT("Diana: Hacking Cooldown Over. Ready to hack!"));
}

float ADiana::GetHackingRemainingTime() const
{
	// 타이머 핸들을 이용해 남은 시간을 구해 리턴
	return GetWorldTimerManager().GetTimerRemaining(HackingTimerHandle);
}

void ADiana::AddEXP(float Amount)
{
	if (Amount <= 0.0f) return;

	CurrentEXP += Amount;
	UE_LOG(LogTemp, Warning, TEXT("Diana Gained EXP: +%f (Current: %f / %f)"), Amount, CurrentEXP, MaxEXP);

	// 경험치가 통을 넘어설 경우 레벨업 처리 (연속 레벨업 가능하도록 while문 사용)
	while (CurrentEXP >= MaxEXP)
	{
		CurrentEXP -= MaxEXP; // 초과분 이월
		LevelUp();
	}
}

void ADiana::LevelUp()
{
	CurrentLevel++;

	// 레벨업 보상: MP 최대치(MaxMP) 증가 및 현재 MP를 최대치로 회복
	MaxMP += 20.0f;    // 원하는 만큼 마나 최대치 증가량 조절 가능
	CurrentMP = MaxMP; // 회복

	// 레벨업 요구 경험치량 증가 (예: 다음 레벨은 기존 요구량의 1.5배)
	MaxEXP *= 1.5f;

	UE_LOG(LogTemp, Warning, TEXT("Diana Level Up! New Level: %d, Max MP: %f"), CurrentLevel, MaxMP);
}
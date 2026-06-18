// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ST_WeaponData.h"
#include "ST_ItemData.h"
#include "Diana.h"
#include "PlayerHue.generated.h"


UCLASS()
class PROJECTP260514_API APlayerHue : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerHue();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// 블루프린트에서 세팅한 무기 데이터를 C++로 실시간 공유받을 그릇
	UPROPERTY(BlueprintReadWrite, Category = "Weapon")
	FWeaponData CurrentWeaponData;

	// 연사와 지속 레이저 메커니즘을 정교하게 제어할 타이머 핸들 및 발사 상태 변수
	FTimerHandle FireTimerHandle;
	bool bIsFiring = false;

	// 화면 정중앙으로 진짜 레이저(Line Trace)를 쏘고 데미지를 주는 핵심 알맹이 함수
	void ExecuteShot();

	// 탄약 UI
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> AmmoWidgetClass;

	UPROPERTY()
	UUserWidget* AmmoWidget;

	// 해킹 미니게임 퍼즐 UI 클래스
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> HackingWidgetClass;

	// 애니메이션이 견착 상태인지 확인
	UPROPERTY(BlueprintReadOnly, Category = "Weapon")
	bool bIsAimed = false;

	// 사망 상태인지 확인
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Stat")
	bool bIsDead = false;

	// 장전 중 먹은 탄약 수치를 잠시 보관할 변수
	float ReservedAmmoFromItem = 0.0f;
private:
	//부드러운 반동을 위해 남은 반동량을 저장할 변수
	float TargetRecoilPitch = 0.0f;
	float TargetRecoilYaw = 0.0f;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// 기본 무기 데미지에 합산될 데미지
	UPROPERTY(BlueprintReadWrite, Category = "Player Stat")
	float BonusDamage = 0.0f;

	// 플레이어 방어력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stat")
	float PlayerDefense = 0.0f;

	// 플레이어 체력(최대체력)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stat")
	float MaxPlayerHp;

	// 현재 탄약
	UPROPERTY(BlueprintReadOnly, Category = "Weapon")
	float CurrAmmo;

	// 플레이어 체력(현재체력)
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player Stat")
	float CurrPlayerHp;

	// 장전 중인지 체크
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bIsReloading = false;
	
	// 재장전 애니메이션 몽타주
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	class UAnimMontage* ReloadMontage;

	// 사망 애니메이션 몽타주
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	class UAnimMontage* DeadMontage;

	// 사망 UI 클래스
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> DeathWidgetClass;

	// Home맵으로 돌아가기 위한 필요한 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
	FName HomeLevelName;

	// 화면에 띄운 데스 위젯을 기억해둘 변수
	UPROPERTY()
	UUserWidget* DeathWidgetInstance;

	// 현재 연사 모드가 해제되었는지 여부
	UPROPERTY(BlueprintReadWrite, Category = "Weapon|Skill")
	bool bIsAutoUnlocked = false;

	// 현재 레이저 모드가 해제되었는지 여부
	UPROPERTY(BlueprintReadWrite, Category = "Weapon|Skill")
	bool bIsLaserUnlocked = false;

	UPROPERTY(BlueprintReadWrite, Category = "Stat")
	bool bIsCarryingDiana = false;

	// 현재 조준선에 들어온 Enemy 해킹 대상
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hacking")
	class AEnemy* HackingTarget;

	// 현재 조준선에 들어온 키패드 해킹 대상
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Hacking")
	class ADoorKeyPad* KeyPadTarget;

	// 해킹 위젯을 저장할 변수
	UPROPERTY()
	UUserWidget* HackingInstance;

	// 다이애나를 업고 있는지 체크하는 변수
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "UI")
	bool bIsLowHPUiVisible = false;

	// 퀘스트 UI 클래스
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> QuestWidgetClass;

	UPROPERTY(BlueprintReadWrite, Category = "Quest")
	UUserWidget* QuestWidgetInstance;

	// 연사 모드 60초 계산할 타이머
	FTimerHandle AutoUnlockTimer;

	// 레이저 모드 60초 계산할 타이머
	FTimerHandle LaserUnlockTimer;

	// 블루프린트 마우스 클릭 입력 액션(IA_Fire)에서 호출할 함수들
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void StartFire();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void StopFire();

	// 애니메이션 노티파이에서 호출할 함수
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void SetIsFullyAimed(bool bAimed) { bIsAimed = bAimed; }

	//아이템을 먹었을 때 호출될 함수
	UFUNCTION(BlueprintCallable, Category = "Item")
	bool ApplyItemEffect(EItemType ItemType, float value);

	// 3. 애니메이션이 끝났을 때 호출되어 실제 탄약을 채워줄 함수
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void CompleteReload();

	//30초 뒤에 데미지 버프를 해제할 함수
	UFUNCTION()
	void RemoveBonusDamage(float RemoveValue);

	UFUNCTION()
	void RemoveBonusDefense(float RemoveValue);

	// 다이애나가 스킬을 쓰면 블루프린트에서 호출할 잠금해제 함수
	UFUNCTION(BlueprintCallable, Category = "Weapon|Skill")
	void UnlockAutoMode();

	// 60초 뒤에 자동으로 호출되어 다시 연사 모드를 잠글 함수
	UFUNCTION(BlueprintCallable, Category = "Weapon|Skill")
	void LockAutoMode();

	// 다이애나가 스킬을 쓰면 블루프린트에서 호출할 잠금해제 함수
	UFUNCTION(BlueprintCallable, Category = "Weapon|Skill")
	void UnlockLaserMode();

	// 60초 뒤에 자동으로 호출되어 다시 레이저모드를 잠글 함수
	UFUNCTION(BlueprintCallable, Category = "Weapon|Skill")
	void LockLaserMode();

	// Q키를 누르면 실행될 함수
	UFUNCTION(BlueprintCallable, Category = "Skill")
	void RequestWeaponUnlock();

	// F키를 누르면 실행될 해킹 시작 함수
	UFUNCTION(BlueprintCallable, Category = "Hacking")
	void StartHacking();

	// 해킹 정지 함수
	UFUNCTION(BlueprintCallable, Category = "Hacking")
	void StopHacking();

	// 해킹 성공 함수
	UFUNCTION(BlueprintCallable, Category = "Hacking")
	void HackingSuccess();

	// 사망 시 실행될 함수
	UFUNCTION(BlueprintCallable, Category = "Stat")
	void PlayerDie();

	// 리스폰 시 실행될 함수
	UFUNCTION(BlueprintCallable, Category = "Stat")
	void RespawnPlayer();

	// 애니메이션 노파이에서 호출될 함수
	UFUNCTION(BlueprintCallable, Category = "Death")
	void ShowDeathUI();

	// 해킹 연출을 블루프린트에서 처리하기 위한 함수
	// 혹시 모르니 다이애나와 적에 정보를 넘겨줌.
	UFUNCTION(BlueprintImplementableEvent, Category = "Hacking")
	void OnHackingSequenceStarted(AEnemy* TargetEnemy, ADiana* Diana);

	// 키패드 해킹 연출 블루프린트에서 처리
	UFUNCTION(BlueprintImplementableEvent, Category = "Hacking")
	void OnKeypadHackingStarted(ADoorKeyPad* TargetKeypad, ADiana* Diana);

	// 블루프린트에서 연출이 다 끝난 후 UI를 띄우기 위해 호출할 함수
	UFUNCTION(BlueprintCallable, Category = "Hacking")
	void ShowHackingUI();

	// 연사 모드 남은 시간 반환하는 함수
	UFUNCTION(BlueprintPure, Category = "Weapon|Skill")
	float GetAutoRemainingTime() const;

	// 레이저 모드 남은 시간 반환하는 함수
	UFUNCTION(BlueprintPure, Category = "Weapon|Skill")
	float GetLaserRemainingTime() const;

	// 다이애나가 허락했을 때 UI를 띄우기 위한 블루프린트 이벤트
	UFUNCTION(BlueprintImplementableEvent, Category = "Skill")
	void ShowWeaponSelectUi();

	// MP가 부족시 대사를 띄울 함수
	UFUNCTION(BlueprintImplementableEvent, Category = "Weapon|Skill")
	void OnWeaponUnlockFailed(FName DialogueRowName);

	// 모드 해제시 나올 대사를 띄울 함수
	UFUNCTION(BlueprintImplementableEvent, Category = "Weapon|Skill")
	void OnWeaponUnlockSuccess(FName DialogueRowName);

	// 타이머 종료 시 무기 머티리얼/UI를 원래대로 돌리기 위해 블루프린트로 보내는 신호
	UFUNCTION(BlueprintImplementableEvent, Category = "Weapon|Skill")
	void OnWeaponModeForcedToSingle();

	// 체력이 30% 이하로 떨어지거나 복구될 때 블루프린트에 알려줄 이벤트
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void OnPlayerLowHPStatusChanged(bool bIsLowHP);

	// 퀘스트 UI를 띄우는 함수 (필요할 때 호출)
	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowQuestUI();

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
protected:
		// 매 프레임 화면 중앙에 적이 있는지 검사하는 센서 함수
		void UpdateTargetingSensor();
};

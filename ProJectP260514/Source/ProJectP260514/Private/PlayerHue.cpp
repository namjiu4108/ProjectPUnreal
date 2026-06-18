// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHue.h"
#include "Enemy.h"
#include "Kismet/GameplayStatics.h" // 데미지 적용을 위한 헤더
#include "DrawDebugHelpers.h"       // 화면에 디버그 레이저 선을 그리기 위한 헤더
#include "Particles/ParticleSystemComponent.h"
#include "Diana.h"
#include "DoorKeyPad.h"
#include "Blueprint/UserWidget.h" // UUserWidget 사용하기 위한 헤더
#include "PragmataGameInstance.h"

// Sets default values
APlayerHue::APlayerHue()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APlayerHue::BeginPlay()
{
	Super::BeginPlay();
	
	if (AmmoWidgetClass)
	{
		AmmoWidget = CreateWidget<UUserWidget>(GetWorld(), AmmoWidgetClass);
		if (AmmoWidget)
		{
			AmmoWidget->AddToViewport();
		}
	}

	//  게임 시작 시 기본 모드를 강제로 '단발'로 설정하고 상태를 확정
	CurrentWeaponData.FireMode = EFireMode::Single;

	// 게임 데이터 불러오기
	UPragmataGameInstance* GI = Cast<UPragmataGameInstance>(GetGameInstance());

	if (GI && GI->bHasSavedData)
	{
		CurrPlayerHp = GI->SavedPlayerHp;
		CurrAmmo = GI->SavedAmmo;
	}
	else
	{
		// 데이터테이블에서 가져온 MaxAmmo를 현재 탄약으로 초기화
		CurrAmmo = CurrentWeaponData.MaxAmmo;

		// 현재 체력을 최대체력으로 설정
		CurrPlayerHp = MaxPlayerHp;
	}	

	if (GI && GI->bIsQuestActive)
	{
		ShowQuestUI();
	}
}

// Called every frame
void APlayerHue::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 반동 부드럽게 하기
	if (TargetRecoilPitch > 0.001f || FMath::Abs(TargetRecoilYaw) > 0.001f)
	{
		//FInterpTo : 현재 값(0)에서 목표값(Target)까지 부드럽게 다가가는 수치를 계산
		// 마지막 숫자(15.0f)가 반동 속도, 숫자가 작을수록 묵직하고 느리게 올라감.
		float InterpPitch = FMath::FInterpTo(0.0f, TargetRecoilPitch, DeltaTime, 15.0f);
		float InterpYaw = FMath::FInterpTo(0.0f, TargetRecoilYaw, DeltaTime, 15.0f);

		// 쪼개진 수치만큼 실제로 카메라를 부드럽게 움직임
		AddControllerPitchInput(-InterpPitch);
		AddControllerYawInput(InterpYaw);

		// 적용한 만큼 바구니에서 빼줌 (결국 0이 되면 멈춤)
		TargetRecoilPitch -= InterpPitch;
		TargetRecoilYaw -= InterpYaw;
	}

	// [추가된 타겟팅 로직]
	if (bIsAimed)
	{
		// 견착 중이면 센서 가동
		UpdateTargetingSensor();
	}
	else
	{
		// 견착을 풀면 타겟 초기화
		HackingTarget = nullptr;
	}
}

// Called to bind functionality to input
void APlayerHue::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void APlayerHue::StartFire()
{
	// 1. 단순 견착 상태인가?
	// 2. 애니메이션까지 완전히 완료되었는가?
	if (!bIsAimed)
	{
		return;
	}


	if (bIsFiring) return;
	bIsFiring = true;

	// 모드별 발사 주기 계산
	float FireInterval = CurrentWeaponData.FireRate;

	//레이저 모드일때만 주기를 빠르게
	if (CurrentWeaponData.FireMode == EFireMode::Laser)
	{
		FireInterval /= 1.5f;
	}

	// 1. 단발 모드일 때 (Single)
	if (CurrentWeaponData.FireMode == EFireMode::Single)
	{
		ExecuteShot();
		bIsFiring = false; // 단발은 한 번 쏘면 사격 상태 즉시 종료
	}
	// 2. 연사(Automatic) 또는 지속형 빔 레이저(Laser) 모드일 때
	else
	{
		// 첫 발은 즉시 발사
		ExecuteShot();

		// 데이터 테이블에 적힌 Fire Rate(발사 주기) 간격으로 ExecuteShot 함수를 무한 반복 호출
		// 프레임 저하(렉)가 걸려도 싱크가 절대 밀리지 않는 언리얼 표준 방식
		GetWorldTimerManager().SetTimer(FireTimerHandle, this, &APlayerHue::ExecuteShot, FireInterval, true);
	}
}

void APlayerHue::StopFire()
{
	bIsFiring = false;
	// 마우스를 떼면 가동 중이던 무한 루프 연사 타이머를 안전하게 꺼버림
	GetWorldTimerManager().ClearTimer(FireTimerHandle);
}

void APlayerHue::ExecuteShot()
{
	if (!GetWorld()) return;

	UE_LOG(LogTemp, Warning, TEXT("MuzzleFlash: %s"),
		CurrentWeaponData.MuzzleFlashEffect ? TEXT("OK") : TEXT("NULL"));
	UE_LOG(LogTemp, Warning, TEXT("Tracer: %s"),
		CurrentWeaponData.BulletTracerEffect ? TEXT("OK") : TEXT("NULL"));
	UE_LOG(LogTemp, Warning, TEXT("MaxRange: %f"), CurrentWeaponData.MaxRange);


	// 현재 모드에 따라 탄약 소모량 결정
	float Consumption = 0.0f;
	switch (CurrentWeaponData.FireMode)
	{
	case EFireMode::Single:
		Consumption = CurrentWeaponData.BaseAmmoConsumption;
		break;
	case EFireMode::Automatic:
		Consumption = CurrentWeaponData.AutoAmmoConsumption;
		break;
	case EFireMode::Laser:
		Consumption = CurrentWeaponData.LaserAmmoConsumption;
		break;
	}

	// 탄약 체크 -> 발사가 가능한가
	if (CurrAmmo < Consumption)
	{
		//탄약이 부족하면 사격 중단
		StopFire();
		return;
	}
	// 탄약 차감
	CurrAmmo -= Consumption;

	// 사격 애니메이션(몽타주) 재생
	if (CurrentWeaponData.FireMontage)
	{
		PlayAnimMontage(CurrentWeaponData.FireMontage);
	}

	// 2. 화면 반동(카메라 흔들림) 적용
	if (CurrentWeaponData.RecoilPitch > 0.0f)
	{
		// 상하 반동 (언리얼에서 Pitch 음수(-)가 위를 향함)
		float RandomPitch = FMath::RandRange(CurrentWeaponData.RecoilPitch * 0.6f, CurrentWeaponData.RecoilPitch);
		
		//AddControllerPitchInput(-RandomPitch);

		// 좌우 반동 (설정한 상하 반동 값의 30% 내에서 좌우 무작위 흔들림)
		float RandomYaw = FMath::RandRange(-CurrentWeaponData.RecoilPitch * 0.3f, CurrentWeaponData.RecoilPitch * 0.3f);

		//AddControllerYawInput(RandomYaw);

		// 즉시 회전시키지 않고, 변수에 목표치만 쌓아둠
		TargetRecoilPitch += RandomPitch;
		TargetRecoilYaw += RandomYaw;
	}

	// 1. 카메라 관리하는 카메라 매니저 가져오기
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC || !PC->PlayerCameraManager) return;

	// 2. 캐릭터 기준이 아니라, 카메라 렌즈 위치의 좌표와 회전값 가져오기
	FVector CameraLocation = PC->PlayerCameraManager->GetCameraLocation();
	FVector CameraForward = PC->PlayerCameraManager->GetCameraRotation().Vector();

	// 3. 카메라 정중앙에서부터 사정거리(MaxRange)만큼 뻗어나간 최종 가상의 목적지 계산
	FVector EndLocation = CameraLocation + (CameraForward * CurrentWeaponData.MaxRange);

	// 4. 충돌 판정 세팅 (플레이어 본인 몸통 무시)
	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	// 5. 카메라 정중앙 시선으로 가상의 보이지 않는 레이저 센서 발사!
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, CameraLocation, EndLocation, ECC_Visibility, QueryParams);

	// 6. 캐릭터 메쉬에 붙어있는 총구 소켓의 실제 월드 좌표를 가져옴
	FVector MuzzleLocation = GetMesh()->GetSocketLocation(TEXT("Muzzle"));

	// 7. 충돌한 곳이 있다면 그 충돌 지점까지, 허공에 쐈다면 최종 맥스 사거리 위치까지를 끝점으로 잡음.
	FVector BeamEndLocation = bHit ? HitResult.ImpactPoint : EndLocation;

	//// 8. 화면에 눈으로 볼 수 있는 빨간색 레이저 선을 1.5초 동안 잔상으로 그림
	//DrawDebugLine(GetWorld(), MuzzleLocation, BeamEndLocation, FColor::Red, false, 1.5f, 0, 1.5f);

	// 1. 총구 화염 스폰 (캐릭터 메쉬의 총구 소켓 위치에 고정시켜서 터뜨립니다)
	if (CurrentWeaponData.MuzzleFlashEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(
			CurrentWeaponData.MuzzleFlashEffect,
			GetMesh(),
			TEXT("Muzzle") // 소총 총구 소켓 이름
		);
	}

	// 2. 눈에 보이는 총알 궤적/레이저 선 스폰 (총구 위치에 생성)
	if (CurrentWeaponData.BulletTracerEffect)
	{
		// 총구에서 실제 타점(BeamEndLocation)을 향하는 방향으로 회전값 계산
		FRotator TracerRotation = (BeamEndLocation - MuzzleLocation).Rotation();

		UParticleSystemComponent* Tracer = UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			CurrentWeaponData.BulletTracerEffect,
			MuzzleLocation, // 정확한 총구 위치 적용
			TracerRotation      // 정확한 정면 방향 적용
		);

		if (Tracer)
		{
			// 파티클의 빔 Target 파라미터에 라인트레이스가 충돌한 진짜 타점을 전달
			Tracer->SetVectorParameter(TEXT("Target"), BeamEndLocation);

			// 자동 파괴 설정 (컴포넌트가 수명을 다하면 자동으로 제거되도록)
			Tracer->bAutoDestroy  = true;

			// 2. 0.05초 뒤에 이 컴포넌트를 즉시 파괴하도록 타이머 설정
			FTimerHandle TimerHandle;
			GetWorldTimerManager().SetTimer(TimerHandle, [Tracer]()
				{
					if (Tracer)
					{
						Tracer->DestroyComponent();
					}
				}, 0.05f, false);
		}
	}

	// 3. 무언가 부딪혔다면 피격 스파크(Impact) 스폰
	if (bHit && CurrentWeaponData.ImpactEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			CurrentWeaponData.ImpactEffect,
			HitResult.ImpactPoint,
			HitResult.ImpactNormal.Rotation() // 벽이나 적이 바라보는 반사 각도로 스파크가 튀게 설정
		);
	}

	// 9. 무언가 레이저에 닿았다면 데미지 프레임워크 가동
	if (bHit)
	{
		AActor* HitActor = HitResult.GetActor();
		if (HitActor)
		{
			// 최종 데미지 = 기본 무기 데미지 + 아이템 추가 데미지
			float FinalDamage = CurrentWeaponData.Damage + BonusDamage;

			// 언리얼 엔진 기본 데미지 시스템으로 데이터 테이블 수치만큼 적의 피를 깎음
			UGameplayStatics::ApplyDamage(HitActor, FinalDamage, GetController(), this, UDamageType::StaticClass());

			// 로그 창에 맞은 대상의 이름과 데미지 수치 출력
			UE_LOG(LogTemp, Warning, TEXT("Hit Target: %s, Applied Damage: %f"), *HitActor->GetName(), FinalDamage);
		}
	}
}

bool APlayerHue::ApplyItemEffect(EItemType ItemType, float value)
{
	switch (ItemType)
	{
	case EItemType::Ammo:
		// 1. 탄약이 이미 꽉 차 있으면 튕겨냄 (아이템 파괴 안 됨)
		if (CurrAmmo >= CurrentWeaponData.MaxAmmo)
		{
			return false;
		}

		// 2. 장전 및 견착 해제 처리
		bIsAimed = false;       // 강제로 견착 풀기
		bIsReloading = true;    // 장전 상태로 전환

		// 3. 획득한 탄약량 임시 저장
		ReservedAmmoFromItem = value;

		// 장전 애니메이션 재생
		if (ReloadMontage)
		{
			PlayAnimMontage(ReloadMontage);
		}

		return true;
		
	case EItemType::DamageUp:
	{
		// 데미지 일시 증가 (30초)
		BonusDamage += value;
		UE_LOG(LogTemp, Warning, TEXT("Bonus Damage : %f"), BonusDamage);

		// 30초 뒤에 실행될 타이머 설정
		FTimerHandle BuffTimer;

		// RemoveBonusDamage 함수에 방금 올린 value 값을 그대로 전달하도록 델리게이트 생성
		FTimerDelegate TimerDel = FTimerDelegate::CreateUObject(this, &APlayerHue::RemoveBonusDamage, value);

		// 30초 뒤에 반복없이(false) 타이머 실행
		GetWorldTimerManager().SetTimer(BuffTimer, TimerDel, 30.0f, false);
		return true;
	}

	case EItemType::DefenseUp:
	{
		// 방어력 일시 증가 (30초)
		PlayerDefense += value;
		UE_LOG(LogTemp, Warning, TEXT("player defense : %f"), PlayerDefense);

		// 30초 뒤에 실행될 타이머 설정
		FTimerHandle BuffTimer;

		// RemoveBonusDefense 함수에 방금 올린 value 값을 그대로 전달하도록 델리게이트 생성
		FTimerDelegate TimerDel = FTimerDelegate::CreateUObject(this, &APlayerHue::RemoveBonusDefense, value);

		// 30초 뒤에 반복없이(false) 타이머 실행
		GetWorldTimerManager().SetTimer(BuffTimer, TimerDel, 30.0f, false);
		return true;
	}

	case EItemType::RecoveryHp:
	{
		// 플레이어이 체력이 최대라면 먹지 않음
		if (CurrPlayerHp >= MaxPlayerHp)
		{
			return false;
		}

		// 최대체력을 넘지 않도록 Clamp를 사용하여 회복
		CurrPlayerHp = FMath::Clamp(CurrPlayerHp + value, 0.0f, MaxPlayerHp);

		float HpRatio = CurrPlayerHp / MaxPlayerHp;
		if (HpRatio > 0.3f) // 30% 위로 회복되었다면
		{
			if (bIsLowHPUiVisible)
			{
				bIsLowHPUiVisible = false;
				OnPlayerLowHPStatusChanged(false); // 블루프린트에 "안전" 신호 전달
			}
		}
		return true;
	}

	case EItemType::RecoveryMp:
	{
		// 월드에 있는 다이애나를 찾음
		AActor* FoundDiana = UGameplayStatics::GetActorOfClass(GetWorld(), ADiana::StaticClass());
		ADiana* DianaRef = Cast<ADiana>(FoundDiana);

		// 플레이어이 체력이 최대라면 먹지 않음
		if (DianaRef)
		{
			if (DianaRef->CurrentMP >= DianaRef->MaxMP)
			{
				return false;
			}

			// 최대 MP를 넘지 않도록 Clamp를 사용하여 회복
			DianaRef->CurrentMP = FMath::Clamp(DianaRef->CurrentMP + value, 0.0f, DianaRef->MaxMP);
			UE_LOG(LogTemp, Warning, TEXT("Diana MP Recovered! Current MP: %f"), DianaRef->CurrentMP);

			return true;
		}
		return false;
	}
	}
	return false;
}

void APlayerHue::CompleteReload()
{
	// 1. 장전 상태 해제 (이제 다시 견착 가능)
	bIsReloading = false;

	// 2. 탄약 채우기 (현재 탄약 + 아까 먹은 탄약을 더하되, 최대 탄약을 넘지 못하도록 FMath::Clamp 사용)
	CurrAmmo = FMath::Clamp(CurrAmmo + ReservedAmmoFromItem, 0.0f, CurrentWeaponData.MaxAmmo);
	UE_LOG(LogTemp, Warning, TEXT("AMMO Restored! Current Ammo : %f"), CurrAmmo);

	// 3. 임시 변수 초기화
	ReservedAmmoFromItem = 0.0f;
}

void APlayerHue::RemoveBonusDamage(float RemoveValue)
{
	// 일시적으로 올랐던 수치만큼 빼기
	BonusDamage -= RemoveValue;

	// 혹시라도 수치가 꼬여서 0보다 작아지는것을 방지
	if (BonusDamage < 0.0f) BonusDamage = 0.0f;

	UE_LOG(LogTemp, Warning, TEXT("Down Bonus Damage : %f"), BonusDamage);
}

void APlayerHue::RemoveBonusDefense(float RemoveValue)
{
	// 일시적으로 올랐던 수치만큼 빼기
	PlayerDefense -= RemoveValue;

	// 혹시라도 수치가 꼬여서 0보다 작아지는것을 방지
	if (PlayerDefense < 0.0f) PlayerDefense = 0.0f;

	UE_LOG(LogTemp, Warning, TEXT("Down player defense : %f"), PlayerDefense);
}

void APlayerHue::UnlockAutoMode()
{
	bIsAutoUnlocked = true; // 무기 잠금 해제
	
	// 기존에 돌아가고 있던 타이머가 있다면 초기화
	GetWorldTimerManager().ClearTimer(AutoUnlockTimer);

	// 60초 뒤에 LockWeaponModes  함수를 실행하도록 타이머 설정
	GetWorldTimerManager().SetTimer(AutoUnlockTimer, this, &APlayerHue::LockAutoMode, 60.0f, false);

	UE_LOG(LogTemp, Warning, TEXT("Diana: Weapon Limiter Released for 60 seconds!"));
}

void APlayerHue::LockAutoMode()
{
	bIsAutoUnlocked = false; // 무기 잠금

	// 만약 연사 모드를 쓰고 있었다면 단발로 강제 복귀
	if (CurrentWeaponData.FireMode == EFireMode::Automatic)
	{
		// 만약 플레이어가 현재 연사나 레이저를 쓰고 있었다면, 강제로 단발모드로 변경
		CurrentWeaponData.FireMode = EFireMode::Single;

		// 쏘던 중 모드가 변경이 되면 에러가 발생할 수 있기에 강제적으로 발사 중지
		StopFire();

		// 머티리얼 및 UI 복구 신호
		OnWeaponModeForcedToSingle();
	}
	UE_LOG(LogTemp, Warning, TEXT("60 seconds End! Return Single mode"));

}

void APlayerHue::UnlockLaserMode()
{
	bIsLaserUnlocked = true; // 무기 잠금 해제

	// 기존에 돌아가고 있던 타이머가 있다면 초기화
	GetWorldTimerManager().ClearTimer(LaserUnlockTimer);

	// 60초 뒤에 LockWeaponModes  함수를 실행하도록 타이머 설정
	GetWorldTimerManager().SetTimer(LaserUnlockTimer, this, &APlayerHue::LockLaserMode, 60.0f, false);

	UE_LOG(LogTemp, Warning, TEXT("Diana: Weapon Limiter Released for 60 seconds!"));
}

void APlayerHue::LockLaserMode()
{
	bIsLaserUnlocked = false; // 무기 잠금

	// 만약 레이저 모드를 쓰고 있었다면 단발로 강제 복귀
	if (CurrentWeaponData.FireMode == EFireMode::Laser)
	{
		// 만약 플레이어가 현재 연사나 레이저를 쓰고 있었다면, 강제로 단발모드로 변경
		CurrentWeaponData.FireMode = EFireMode::Single;

		// 쏘던 중 모드가 변경이 되면 에러가 발생할 수 있기에 강제적으로 발사 중지
		StopFire();

		// 머티리얼 및 UI 복구 신호
		OnWeaponModeForcedToSingle();
	}
	UE_LOG(LogTemp, Warning, TEXT("60 seconds End! Return Single mode"));
}

float APlayerHue::GetAutoRemainingTime() const
{
	// 타이머가 작동 중이면 남은 초를 반환
	return GetWorldTimerManager().GetTimerRemaining(AutoUnlockTimer);
}

float APlayerHue::GetLaserRemainingTime() const
{
	// 타이머가 작동 중이면 남은 초를 반환
	return GetWorldTimerManager().GetTimerRemaining(LaserUnlockTimer);
}

void APlayerHue::RequestWeaponUnlock()
{
	// 업고 있지않으면 아무것도 안함
	if (!bIsCarryingDiana) return;

	// PlayerHue 클래스 내부에 다이애나 포인터가 없으므로, 월드에서 다이애나를 찾아옴
	AActor* FoundActor = UGameplayStatics::GetActorOfClass(GetWorld(), ADiana::StaticClass());
	ADiana* DianaRef = Cast<ADiana>(FoundActor);

	if (DianaRef)
	{
		// 요구 MP 설정 
		float MinRequiredMP = 30.0f;

		// 다이애나의 현재 MP가 부족하다면?
		if (DianaRef->CurrentMP < MinRequiredMP)
		{
			// MP가 부족하다는 사실을 다이애나에게 기억시킴
			DianaRef->bIsShortOfMP = true;

			// 실패 대사 띄우기 신호 발송
			OnWeaponUnlockFailed(FName("7"));

			// 여기서 함수 강제 종료 (UI 호출 안 됨)
			return;
		}
		// 모드 해제 대사 띄우기 신호
		OnWeaponUnlockSuccess(FName("9"));
	}

	// Q를 누르면 mp 소모 없이 일단 선택 UI 호출
	ShowWeaponSelectUi();
}

float APlayerHue::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// 이미 죽었거나 데미지가 없으면 무시
	if (CurrPlayerHp <= 0.0f || ActualDamage <= 0.0f)
	{
		return 0.0f;
	}

	// 플레이어 방어력(PlayerDefense) 반영해서 최종 데미지 계산 (최소 데미지는 0)
	float FinalDamage = FMath::Max(0.0f, ActualDamage - PlayerDefense);

	// 현재 체력 감소
	CurrPlayerHp -= FinalDamage;

	// 잘 깎이는지 확인용 로그
	UE_LOG(LogTemp, Warning, TEXT("Player Hue Took Damage! Original: %f, Defense: %f, Final: %f, Current HP: %f"),
		ActualDamage, PlayerDefense, FinalDamage, CurrPlayerHp);

	float HpRatio = CurrPlayerHp / MaxPlayerHp; // 100.0f 대신 MaxPlayerHp 사용
	if (HpRatio <= 0.3f)
	{
		if (!bIsLowHPUiVisible)
		{
			bIsLowHPUiVisible = true;
			OnPlayerLowHPStatusChanged(true); // 블루프린트에 "위험" 신호 전달
		}
	}

	// 체력이 0 이하가 되면 사망 처리
	if (CurrPlayerHp <= 0.0f && !bIsDead)
	{
		PlayerDie();
		UE_LOG(LogTemp, Error, TEXT("Player is Dead!"));
	}

	return FinalDamage;
}

void APlayerHue::UpdateTargetingSensor()
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC || !PC->PlayerCameraManager) return;

	FVector CameraLocation = PC->PlayerCameraManager->GetCameraLocation();
	FVector CameraForward = PC->PlayerCameraManager->GetCameraRotation().Vector();

	// 무기 사거리만큼 타겟팅 거리 설정
	FVector EndLocation = CameraLocation + (CameraForward * CurrentWeaponData.MaxRange);

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this); // 내 몸은 무시

	// 보이지 않는 센서 레이저 발사!
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, CameraLocation, EndLocation, ECC_Visibility, QueryParams);

	if (bHit)
	{
		AActor* HitActor = HitResult.GetActor();
		// 부딪힌 대상이 AEnemy 클래스(또는 그 자식)인지 확인
		AEnemy* FoundEnemy = Cast<AEnemy>(HitActor);

		if (FoundEnemy && !FoundEnemy->bIsDead)
		{
			// 살아있는 적이라면 타겟으로 지정
			if (HackingTarget != FoundEnemy)
			{
				HackingTarget = FoundEnemy;
				KeyPadTarget = nullptr; // 적을 조준중이면 키패드 타겟 해제
				// 디버그 로그
				UE_LOG(LogTemp, Warning, TEXT("Target Locked: %s"), *HackingTarget->GetName());
			}
		}
		else
		{
			ADoorKeyPad* FoundKeyPad = Cast<ADoorKeyPad>(HitActor);

			if (FoundKeyPad)
			{
				KeyPadTarget = FoundKeyPad;
				// 적이 아니거나 죽은 적이면 타겟 해제
				HackingTarget = nullptr;
			}
			else
			{
				// 둘다 아니면 초기화
				HackingTarget = nullptr;
				KeyPadTarget = nullptr;
			}
		}
	}
	else
	{
		// 허공을 보고 있으면 타겟 해제
		HackingTarget = nullptr;
		KeyPadTarget = nullptr;
	}
}

void APlayerHue::StartHacking()
{
	// 1. 다이애나를 업고 있지 않거나, 타겟(Enemy, KeyPad)이 없으면 실행 불가
	if (!bIsCarryingDiana || (!HackingTarget&&!KeyPadTarget)) return;

	// 2. 월드에서 다이애나 찾기
	AActor* FoundActor = UGameplayStatics::GetActorOfClass(GetWorld(), ADiana::StaticClass());
	ADiana* DianaRef = Cast<ADiana>(FoundActor);

	// 3. 다이애나 쿨타임 체크 및 시작
	if (DianaRef && DianaRef->bCanHack)
	{
		// 4. 다이애나 해킹 시작
		DianaRef->StartHackingCooldown();
		
		// Enemy, KeyPad일때 각각 맞는 이벤트 호출
		if (HackingTarget)
		{
			// 여기서 UI를 바로 띄우지 않고, 블루프린트의 연출 이벤트로 신호
			// 이때 타겟 적(HackingTarget)과 다이애나 정보(DianaRef)를 같이 전달
			OnHackingSequenceStarted(HackingTarget, DianaRef);
		}
		else if (KeyPadTarget)
		{
			OnKeypadHackingStarted(KeyPadTarget, DianaRef);
		}
	}
}

void APlayerHue::StopHacking()
{
	if (HackingInstance)
	{
		HackingInstance->RemoveFromParent();
		HackingInstance = nullptr; // 변수 초기화
	}

	// 입력 모드 복구(게임 전용)
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC)
	{
		// 움직이면서 UI 클릭가능하게 함
		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = false; // 마우스 커서를 보dl지 않게함

	}
}

void APlayerHue::HackingSuccess()
{
	// 현재 조준 중인 해킹 대상이 Enemy의 경우
	if (HackingTarget)
	{
		UE_LOG(LogTemp, Warning, TEXT("HackingSuccess: Target = %s"), *HackingTarget->GetName());
		// Enemy 클래스에 만들어줄 디버프 함수 호출
		HackingTarget->ApplyHackingDebuff();
	}
	else if (KeyPadTarget) // 조준 중인 해킹 대상이 키패드의 경우
	{
		KeyPadTarget->OpenDoor();
	}

	// 해킹에 성공했으니 위젯 없애기
	StopHacking();

	// 다음 해킹을 위해 타겟 초기화
	HackingTarget = nullptr;
	KeyPadTarget = nullptr;
}

void APlayerHue::PlayerDie()
{
	bIsDead = true;
	// 로그 추가: 함수가 실행되는지 확인
	UE_LOG(LogTemp, Warning, TEXT("PlayerDie Function Started"));
	// 조작 차단 : 일시정지
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC)
	{
		DisableInput(PC);
	}

	// 사망 애니메이션
	if (DeadMontage)
	{
		PlayAnimMontage(DeadMontage);
		UE_LOG(LogTemp, Warning, TEXT("Playing DeadMontage"));
	}
	else
	{
		// 만약 이 로그가 뜬다면 블루프린트에서 DeadMontage를 설정하지 않은 것입니다!
		UE_LOG(LogTemp, Error, TEXT("DeadMontage is NULL! Please set it in Blueprint."));
	}
	// 4초 뒤 Home맵에서 리스폰
	FTimerHandle RespawnTime;
	GetWorldTimerManager().SetTimer(RespawnTime, this, &APlayerHue::RespawnPlayer, 10.0f, false);
}

void APlayerHue::RespawnPlayer()
{
	// 실제 플레이어 탄약 최대로
	CurrAmmo = CurrentWeaponData.MaxAmmo;

	// 화면에 떠있는 사망 위젯 없애기
	if (DeathWidgetInstance)
	{
		DeathWidgetInstance->RemoveFromParent();
		DeathWidgetInstance = nullptr;
	}

	// 부활할 때 체력을 꽉 채우도록 게임 인스턴스 데이터 갱신
	UPragmataGameInstance* GI = Cast<UPragmataGameInstance>(GetGameInstance());
	if (GI)
	{
		GI->SavedPlayerHp = MaxPlayerHp;
		GI->SavedAmmo = CurrAmmo;
	}
	
	// HOME 레벨로 맵 이동! 
	UGameplayStatics::OpenLevel(this, HomeLevelName);
}

void APlayerHue::ShowDeathUI()
{
	// 로그 추가: 노티파이가 호출되는지 확인
	UE_LOG(LogTemp, Warning, TEXT("AnimNotify_ShowDeathUI Called!"));
	// 사망 UI 화면출력
	if (DeathWidgetClass)
	{
		DeathWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), DeathWidgetClass);
		if (DeathWidgetInstance)
		{
			DeathWidgetInstance->AddToViewport();
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("DeathWidgetClass is NULL in Blueprint!"));
	}
}

void APlayerHue::ShowHackingUI()
{
	// 5. 퍼즐 UI 생성 및 화면에 띄우기
	if (HackingWidgetClass)
	{
		HackingInstance = CreateWidget<UUserWidget>(GetWorld(), HackingWidgetClass);
	
		if (HackingInstance)
		{
			HackingInstance->AddToViewport();
	
			// 5. 입력 모드를 UI 전용으로 변경 (캐릭터 움직임 잠시 정지)
			APlayerController* PC = Cast<APlayerController>(GetController());
			if (PC)
			{
				// 움직이면서 UI 클릭가능하게 함
				FInputModeGameAndUI InputMode;
				InputMode.SetWidgetToFocus(HackingInstance->TakeWidget());
				InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock); // 마우스가 게임창 밖으로 나가는 것을 막지 않음
				
				PC->SetInputMode(InputMode);
				PC->bShowMouseCursor = true; // 마우스 커서를 보이게함
			}
		}
	}
}

void APlayerHue::ShowQuestUI()
{
	if (QuestWidgetClass && !QuestWidgetInstance)
	{
		QuestWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), QuestWidgetClass);
		if (QuestWidgetInstance)
		{
			QuestWidgetInstance->AddToViewport();
		}
	}
}
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h" // 중요: 데이터 테이블(FTableRowBase)을 쓰기 위해 필수
#include "ST_WeaponData.generated.h" // 중요: 파일 이름과 무조건 똑같아야 함

/**
 * 1. 무기 발사 방식 열거형 (Enum)
 * 단발 레이저, 연사 레이저, 프래그마타 스타일의 지속 레이저를 구분
 */
UENUM(BlueprintType)
enum class EFireMode : uint8
{
    Single      UMETA(DisplayName = "Single"),      // 단발 레이저
    Automatic   UMETA(DisplayName = "Automatic"),   // 연사 레이저
    Laser       UMETA(DisplayName = "Laser")        // 지속형 빔 레이저
};

/**
 * 2. 총기 데이터 구조체 (Struct)
 * 반드시 : public FTableRowBase를 상속받아야 에디터에서 데이터 테이블로 변환
 */
USTRUCT(BlueprintType)
struct FWeaponData : public FTableRowBase
{
    GENERATED_BODY()

public:
    // 생성자 (Constructor): 데이터 테이블에 행을 추가할 때 채워질 기본값들을 세팅
    FWeaponData()
        : WeaponName(FText::FromString(TEXT("Default Laser")))
        , FireMode(EFireMode::Single)
        , WeaponMaterial(nullptr)
        , Damage(10.0f)
        , FireRate(0.2f)
        , MaxRange(5000.0f)
        , MuzzleFlashEffect(nullptr)
        , BulletTracerEffect(nullptr)
        , ImpactEffect(nullptr)
        , BaseAmmoConsumption(1.0f)
        , AutoAmmoConsumption(1.0f)
        , LaserAmmoConsumption(2.0f)
        , MaxAmmo(150.f)
        , RecoilPitch(1.0f)
        , UnlockMPCost(0.0f)
        , FireMontage(nullptr)
    {
    }

    // [무기 기본 정보] UI에 띄울 무기의 한글/영문 이름
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Info")
    FText WeaponName;

    // [발사 방식] 단발 / 연사 / 지속 레이저 중 선택
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Info")
    EFireMode FireMode;

    // [무기 외형] 무기를 바꿨을 때 캐릭터 메시의 '4번 슬롯'에 덮어씌울 머티리얼 에셋
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Visual")
    UMaterialInterface* WeaponMaterial;

    // [전투 스펙] 레이저 적중 시 들어갈 데미지 수치
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Stat")
    float Damage;

    // [발사 주기] 단발/연사 시 연사 속도 딜레이초, 지속 레이저일 때는 틱 데미지가 들어가는 주기 초
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Stat")
    float FireRate;

    // [사정거리] Line Trace가 정면으로 뻗어나갈 최대 거리 (센티미터 단위, 5000.0f = 50미터)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Stat")
    float MaxRange;

    // 1. 총구에서 뿜어져 나올 화염 효과 (머독 에셋의 MuzzleFlash 파티클용)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Visual")
    class UParticleSystem* MuzzleFlashEffect;

    // 2. 눈에 보이는 총알 궤적/레이저 선 효과 (머독 에셋의 Tracer / Beam 파티클용)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Visual")
    class UParticleSystem* BulletTracerEffect;

    // 3. 벽이나 적에게 맞았을 때 터지는 스파크 효과 (머독 에셋의 ImpactEffect 파티클용)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Visual")
    class UParticleSystem* ImpactEffect;

    // 최대 탄약
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Stat")
    float MaxAmmo;

    // 단발 탄약 소모량
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Stat")
    float BaseAmmoConsumption;

    // 연사 탄약 소모량
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Stat")
    float AutoAmmoConsumption;

    // 레이저 탄약 소모량
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Stat")
    float LaserAmmoConsumption;

    // 총을 쏠 때 화면이 위로 튀는 반동 세기 (예: 0.5 ~ 1.0)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Stat")
    float RecoilPitch;

    // 모드별 MP 소모량
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Stat")
    float UnlockMPCost;

    // 총을 쏠 때 재생할 몽타주
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Visual")
    class UAnimMontage* FireMontage;
};
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h" 
#include "ST_ItemData.generated.h"

/**
 * 
 */

// 1. 아이템 종류를 구분할 Enum(열거형)
UENUM(BlueprintType)
enum class EItemType : uint8
{
	Ammo UMETA(DisplayName = "Ammo Restore"), // 탄약 보급
	DamageUp UMETA(DisplayName = "Damage Up"), // 데미지 증가
	DefenseUp UMETA(DisplayName = "Defense Up"), // 방어력 증가
	RecoveryMp UMETA(DisplayName = "MP Recovery"), // 다이애나 MP 회복
	RecoveryHp UMETA(DisplayName = "HP Recovery") // 플레이어 HP 회복
};

// 2. 아이템 데이터 구조체
USTRUCT(BlueprintType)
struct  FItemData : public FTableRowBase
{
	GENERATED_BODY()

public:
	FItemData()
		: ItemName(TEXT("Default Item"))
		, ItemType(EItemType::Ammo)
		, ItemValue(30.0f)
		, ItemParticle(nullptr)
		, PickupEffect(nullptr)
		, PickupSound(nullptr)
	{
	}

	// 아이템 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Info")
	FName ItemName;

	// 아이템 종류 (탄약, 데미지, 방어력)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Info")
	EItemType ItemType;

	// 아이템 수치
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Info")
	float ItemValue;

	// 필드에 떨어져 있을 때 보여줄 외형
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Visual")
	class UParticleSystem* ItemParticle;

	// 아이템 먹을 때 터질 파티클 이펙트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Visual")
	class UParticleSystem* PickupEffect;

	//먹을 때 나올 효과음
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Visual")
	class USoundBase* PickupSound;
};
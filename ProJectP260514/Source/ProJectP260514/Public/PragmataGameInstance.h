// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ST_QuestData.h" // 퀘스트 구조체 사용을 위해 추가
#include "PragmataGameInstance.generated.h"

class APlayerHue;
class ADiana;

/**
 * 
 */
UCLASS()
class PROJECTP260514_API UPragmataGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	// 맵 이동 한 적이 있는지 확인
	// 게임 맨 처음 시작할 때 HP 0으로 시작하는거 방지
	UPROPERTY(BlueprintReadWrite, Category = "SaveData")
	bool bHasSavedData = false;

	// 플레이어 데이터
	UPROPERTY(BlueprintReadWrite, Category = "SaveData")
	float SavedPlayerHp;

	// 플레이어 데이터
	UPROPERTY(BlueprintReadWrite, Category = "SaveData")
	float SavedAmmo;

	// 다이애나 데이터
	// 플레이어 데이터
	UPROPERTY(BlueprintReadWrite, Category = "SaveData")
	int32 SavedDianaLevel;

	UPROPERTY(BlueprintReadWrite, Category = "SaveData")
	float SavedDianaExp;

	UPROPERTY(BlueprintReadWrite, Category = "SaveData")
	float SavedDianaMaxExp;

	UPROPERTY(BlueprintReadWrite, Category = "SaveData")
	float SavedDianaMaxMp;

	UPROPERTY(BlueprintReadWrite, Category = "SaveData")
	float SavedDianaCurrentMp;

	// 퀘스트 수락 여부를 저장하는 Map
	// Key: 퀘스트 ID (FName), Value: 수락했는지 여부 (bool)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QuestSystem")
	TMap<FName, bool> AcceptedQuests;

	// 현재 진행 중인 퀘스트 데이터
	UPROPERTY(BlueprintReadWrite, Category = "Quest")
	FQuestData CurrentQuest;

	// 퀘스트가 활성화되었는지 확인하는 플래그
	UPROPERTY(BlueprintReadWrite, Category = "Quest")
	bool bIsQuestActive = false;

	// 관리실 시스템을 수동 복구했는지 여부
	UPROPERTY(BlueprintReadWrite, Category = "Quest")
	bool bIsSystemRestored = false;


	UFUNCTION(BlueprintCallable, Category = "SaveData")
	void SaveCurrentGameData(APlayerHue* Player, ADiana* Diana);
};

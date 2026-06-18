// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h" 
#include "ST_QuestData.generated.h"
/**
 * 
 */

// 언리얼에서 데이터 테이블의 행(Row)으로 쓰려면 반드시 'FTableRowBase'를 상속
USTRUCT(BlueprintType)
struct FQuestData : public FTableRowBase
{
	GENERATED_BODY();
public:
    // 퀘스트 고유 ID
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest|System")
    FName QuestID;

    // 퀘스트 제목 (예: "관리 시스템 복구")
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest|System")
    FText QuestName;

    // 퀘스트 목록에 띄울 목표
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest|System")
    FText QuestObjective; 

    // 퀘스트 내용 (터미널 MainDisplay 화면에 띄울 목표 설명)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest|Dialogue")
    TArray<FText> TerminalDialogues;

    // 해킹 후 시스템 복구까지 기다려야 하는 대기 시간 (초)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
    float WaitTimeForRestore;
};


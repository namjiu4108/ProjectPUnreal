// Fill out your copyright notice in the Description page of Project Settings.


#include "PragmataGameInstance.h"
#include "PlayerHue.h"
#include "Diana.h"

void UPragmataGameInstance::SaveCurrentGameData(APlayerHue* Player, ADiana* Diana)
{
    if (Player)
    {
        // PlayerHue.h의 변수들이 public이거나 Getter가 있다고 가정
        SavedPlayerHp = Player->CurrPlayerHp;
        SavedAmmo = Player->CurrAmmo;
    }

    if (Diana)
    {
        SavedDianaLevel = Diana->CurrentLevel;
        SavedDianaExp = Diana->CurrentEXP;
        SavedDianaMaxExp = Diana->MaxEXP;
        SavedDianaMaxMp = Diana->MaxMP;
        SavedDianaCurrentMp = Diana->CurrentMP;
    }

    bHasSavedData = true;
}

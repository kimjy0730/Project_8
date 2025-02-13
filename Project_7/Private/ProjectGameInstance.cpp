#include "ProjectGameInstance.h"


UProjectGameInstance::UProjectGameInstance()
{
	TotalScore = 0;
	CurrentLevelIndex = 0;
	WaveIndex = 0;
}

void UProjectGameInstance::AddToScore(int32 Amount)
{
	TotalScore += Amount;
}
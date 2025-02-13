#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ProjectGameInstance.generated.h"


UCLASS()
class PROJECT_7_API UProjectGameInstance : public UGameInstance
{
	GENERATED_BODY()
	

public:

	UProjectGameInstance();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "GameData")
	int32 TotalScore;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "GameData")
	int32 CurrentLevelIndex;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "GameData")
	int32 WaveIndex;

	UFUNCTION(BlueprintCallable, Category ="GameData")
	void AddToScore(int32 Amount);
};

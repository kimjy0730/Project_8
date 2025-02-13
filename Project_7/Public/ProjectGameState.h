#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "ProjectGameState.generated.h"

UCLASS()
class PROJECT_7_API AProjectGameState : public AGameState
{
	GENERATED_BODY()


public:
	AProjectGameState();

	virtual void BeginPlay() override;

	UPROPERTY(VIsibleAnyWhere, BluePrintReadWrite, Category = "Score")
	int32 Score;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Count")
	int32 SpawnedCoinCount;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Count")
	int32 CollectedCount;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	float LevelDuration;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	int32 CurrentLevelIndex;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	int32 MaxLevels;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
	TArray<FName> LevelMapNames;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave")
	int32 CurrentWave;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
	int32 ItemToSpawn;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave")
	float WaveDuration;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave")
	int32 MaxWave;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wave")
	int32 WaveIndex;
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, Category = "Time")
	float TotalPlayTime;
	

	FTimerHandle WaveTimerHandle;
	FTimerHandle NextWaveTimerHandle;
	FTimerHandle LevelTimerHandle;
	FTimerHandle HUDUPdateTimerHandel;

	UFUNCTION(BlueprintPure, Category = "Score")
	int32 GetScore() const;
	UFUNCTION(BlueprintCallable, Category = "Score")
	void AddScore(int32 Amount);
	UFUNCTION(BlueprintCallable, Category = "Score")
	void OnGameOver();

	void StartLevel();
	void OnLevelTimeUp();
	void OnCoinCollected();
	void EndLevel();
	void UpdateHUD();
	void StartWave(int32 WaveIndex);
	void WaveSpawnItem();
	void NextWave();
};
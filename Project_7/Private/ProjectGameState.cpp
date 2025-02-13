#include "ProjectGameState.h"
#include "ProjectGameInstance.h"
#include "ProjectController.h"
#include "ProjectCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "SpawnValume.h"
#include "CoinItem.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"


AProjectGameState::AProjectGameState()
{
	Score = 0;
	SpawnedCoinCount = 0;
	CollectedCount = 0;
	LevelDuration = 90.0f;
	CurrentLevelIndex = 0;
	MaxLevels = 3;
	CurrentWave = 0;
	ItemToSpawn = 0;
	WaveDuration = 10.0f;
	MaxWave = 3;
	WaveIndex = 0;
	TotalPlayTime = 0.0f;

}

void AProjectGameState::BeginPlay()
{
	Super::BeginPlay();

	StartLevel();

	GetWorldTimerManager().SetTimer(
		HUDUPdateTimerHandel,
		this,
		&AProjectGameState::UpdateHUD,
		0.1f,
		true
	);
}


int32 AProjectGameState::GetScore() const
{
	return Score;
}

void AProjectGameState::AddScore(int32 Amount)
{

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		UProjectGameInstance* ProjectGameInstance = Cast<UProjectGameInstance>(GameInstance);
		if (ProjectGameInstance)
		{
			ProjectGameInstance->AddToScore(Amount);
		}
	}
}

void AProjectGameState::StartLevel()
{

	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (AProjectController* ProjectPlayerController = Cast< AProjectController>(PlayerController))
		{
			ProjectPlayerController->ShowGameHUD();
		}
	}


	if (UGameInstance* GameInstance = GetGameInstance())
	{
		UProjectGameInstance* ProjectGameInstance = Cast<UProjectGameInstance>(GameInstance);
		if (ProjectGameInstance)
		{
			CurrentLevelIndex = ProjectGameInstance->CurrentLevelIndex;
		}
	}
	TotalPlayTime = GetWorld()->GetTimeSeconds();
	WaveIndex = 0;
	StartWave(WaveIndex);

}

void AProjectGameState::OnLevelTimeUp()
{
	EndLevel();
}

void AProjectGameState::OnCoinCollected()
{
	CollectedCount++;

	UE_LOG(LogTemp, Warning, TEXT("Before increment: SpawnedCoinCount = %d / %d"), SpawnedCoinCount, CollectedCount);

	if (SpawnedCoinCount > 0 && CollectedCount >= SpawnedCoinCount)
	{
		GetWorld()->GetTimerManager().ClearTimer(WaveTimerHandle);
		GetWorld()->GetTimerManager().SetTimer(
			NextWaveTimerHandle,
			this,
			&AProjectGameState::NextWave,
			2.0f,
			false);
	}
}


void AProjectGameState::EndLevel()
{
	GetWorldTimerManager().ClearTimer(LevelTimerHandle);
	

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		UProjectGameInstance* ProjectGameInstance = Cast<UProjectGameInstance>(GameInstance);
		if (ProjectGameInstance)
		{
			AddScore(Score);
			CurrentLevelIndex++;
			ProjectGameInstance->CurrentLevelIndex = CurrentLevelIndex;
		}
	}


	if (CurrentLevelIndex >= MaxLevels)
	{
		OnGameOver();
		return;
	}
	if (LevelMapNames.IsValidIndex(CurrentLevelIndex))
	{
		
		TotalPlayTime += GetWorld()->GetTimeSeconds();;
		
		UGameplayStatics::OpenLevel(GetWorld(), LevelMapNames[CurrentLevelIndex]);
	}
	else
	{
		OnGameOver();
	}

}

void AProjectGameState::OnGameOver()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (AProjectController* ProjectPlayerController = Cast< AProjectController>(PlayerController))
		{
			ProjectPlayerController->SetPause(true);
			ProjectPlayerController->ShowEndMenu();
		}
	}
}

void AProjectGameState::UpdateHUD()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (AProjectController* ProjectPlayerController = Cast< AProjectController>(PlayerController))
		{
			if (UUserWidget* HUDWidget = ProjectPlayerController->GetHUDWidget())
			{
				if (UTextBlock* TimeText = Cast< UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("TimeValue"))))
				{
					TimeText->SetText(FText::FromString(FString::Printf(TEXT("Time: %.1f "), TotalPlayTime + GetWorld()->GetTimeSeconds())));

				}
				if (UTextBlock* WaveTimeText = Cast< UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("WaveTimeValue"))))
				{
					float RemainingTime = GetWorldTimerManager().GetTimerRemaining(WaveTimerHandle);
					WaveTimeText->SetText(FText::FromString(FString::Printf(TEXT("WaveTime: %.1f "), RemainingTime)));

				}
				if (UTextBlock* ScoreText = Cast< UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("ScoreValue"))))
				{
					if (UGameInstance* GameInstance = GetGameInstance())
					{
						UProjectGameInstance* ProjetGameInstance = Cast<UProjectGameInstance>(GameInstance);
						if (ProjetGameInstance)
						{
							ScoreText->SetText(FText::FromString(FString::Printf(TEXT("Score: %d "), ProjetGameInstance->TotalScore)));
						}
					}
				}
				if (UTextBlock* LevelIndexText = Cast< UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Level"))))
				{
					
					LevelIndexText->SetText(FText::FromString(FString::Printf(TEXT("Level: %d"), CurrentLevelIndex + 1)));

				}
				if (UTextBlock* WaveIndexText = Cast< UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("WaveValue"))))
				{

					WaveIndexText->SetText(FText::FromString(FString::Printf(TEXT("Wave : %d"), CurrentWave + 1)));

				}
				if (UTextBlock* HealthIndexText = Cast< UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("HealthValue"))))
				{
					AProjectCharacter* PlayerCharacter = Cast<AProjectCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
					if (PlayerCharacter)
					{
						int32 health = PlayerCharacter->GetHealth();
						HealthIndexText->SetText(FText::FromString(FString::Printf(TEXT("Health : %d"), health)));
					}
				}
			}
		}
	}
}

void AProjectGameState::StartWave(int32 NewWaveIndex)
{
	CurrentWave = NewWaveIndex;
	
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Wave %d Start!!"), CurrentWave + 1));


	WaveSpawnItem();

	GetWorldTimerManager().SetTimer(
		WaveTimerHandle,
		this,
		&AProjectGameState::NextWave,
		WaveDuration,
		false
	);

}

void AProjectGameState::WaveSpawnItem()
{
	ItemToSpawn += 10;
	TArray<AActor*> FoundVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnValume::StaticClass(), FoundVolumes);

	const int32 VolumeCount = FoundVolumes.Num();

	for (int32 i = 0; i < ItemToSpawn; i++)
	{
		if (VolumeCount > 0)
		{
			int32 RandomIndex = FMath::RandRange(0, VolumeCount - 1);
			ASpawnValume* SpawnVolume = Cast<ASpawnValume>(FoundVolumes[RandomIndex]);

			if (SpawnVolume)
			{
				AActor* SpawnedActor = SpawnVolume->SpawnRandomItem();

				if (SpawnedActor)
				{
					if (SpawnedActor->IsA(ACoinItem::StaticClass()))
					{
						SpawnedCoinCount++;
					}
				}
			}
		}
	}
}

void AProjectGameState::NextWave()
{

	TArray<AActor*> FoundItems;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseItem::StaticClass(), FoundItems);

	for (AActor* Item : FoundItems)
	{
		if (Item)
		{
			Item->Destroy();
		}
	}

	if (CurrentWave + 1 < 3)
	{
		StartWave(CurrentWave + 1);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Wave Clear!!!  Start To The Next Level!!"));
		GetWorldTimerManager().SetTimer(
			LevelTimerHandle,
			this,
			&AProjectGameState::OnLevelTimeUp,
			3.0f,
			false
		);
	}
}

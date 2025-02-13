#include "ProjectController.h"
#include "ProjectGameState.h"
#include "ProjectGameInstance.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"


AProjectController::AProjectController()
	: InputMappingContext(nullptr),
	MoveAction(nullptr),
	LookAction(nullptr),
	JumpAction(nullptr),
	SprintAction(nullptr),
	HUDWidgetClass(nullptr),
	HUDWidgetInstance(nullptr),
	MainMenuWidgetClass(nullptr),
	MainMenuWidgetInstance(nullptr)
{
}

void AProjectController::BeginPlay()
{
	Super::BeginPlay();

	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (InputMappingContext)
			{
				Subsystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}

	FString CurrentMapName = GetWorld()->GetMapName();
	if (CurrentMapName.Contains("MenuLevel"))
	{
		ShowMainMenu();
	}

}

UUserWidget* AProjectController::GetHUDWidget() const
{
	return HUDWidgetInstance;
}

void AProjectController::ShowMainMenu()
{
	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->RemoveFromParent();
		HUDWidgetInstance = nullptr;
	}

	if (MainMenuWidgetInstance)
	{
		MainMenuWidgetInstance ->RemoveFromParent();
		MainMenuWidgetInstance = nullptr;
	}

	if (EndMenuWidgetInstance)
	{
		EndMenuWidgetInstance->RemoveFromParent();
		EndMenuWidgetInstance = nullptr;
	}

	if (MainMenuWidgetClass)
	{
		MainMenuWidgetInstance = CreateWidget<UUserWidget>(this, MainMenuWidgetClass);
		if (MainMenuWidgetInstance)
		{
			MainMenuWidgetInstance->AddToViewport();

			bShowMouseCursor = true;
			SetInputMode(FInputModeUIOnly());

		}

		if (UTextBlock* ButtonText = Cast<UTextBlock>(MainMenuWidgetInstance->GetWidgetFromName(TEXT("StartButtonText"))))
		{
			ButtonText->SetText(FText::FromString(TEXT("Start")));	
		}


		if (UTextBlock* ButtonText = Cast<UTextBlock>(MainMenuWidgetInstance->GetWidgetFromName(TEXT("EndButtonText"))))
		{
			ButtonText->SetText(FText::FromString(TEXT("Exit")));
		}
	}
}

void AProjectController::ShowEndMenu()
{
	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->RemoveFromParent();
		HUDWidgetInstance = nullptr;
	}

	if (MainMenuWidgetInstance)
	{
		MainMenuWidgetInstance->RemoveFromParent();
		MainMenuWidgetInstance = nullptr;
	}

	if (EndMenuWidgetInstance)
	{
		EndMenuWidgetInstance->RemoveFromParent();
		EndMenuWidgetInstance = nullptr;
	}

	if (EndMenuWidgetClass)
	{
		EndMenuWidgetInstance = CreateWidget<UUserWidget>(this, EndMenuWidgetClass);
		if (EndMenuWidgetInstance)
		{
			EndMenuWidgetInstance->AddToViewport();

			bShowMouseCursor = true;
			SetInputMode(FInputModeUIOnly());

		}
	
		if (UTextBlock* ButtonText = Cast<UTextBlock>(EndMenuWidgetInstance->GetWidgetFromName(TEXT("ReStartButtonText"))))
		{
			ButtonText->SetText(FText::FromString(TEXT("Restart")));
		}


		if (UTextBlock* ButtonText = Cast<UTextBlock>(EndMenuWidgetInstance->GetWidgetFromName(TEXT("GameExitButtonText"))))
		{
			ButtonText->SetText(FText::FromString(TEXT("Back to Main Menu")));
		}

		UFunction* PlayAnimFunc = EndMenuWidgetInstance->FindFunction(FName("GameOverAnim"));
		if (PlayAnimFunc)
		{
			EndMenuWidgetInstance->ProcessEvent(PlayAnimFunc, nullptr);
		}

		UFunction* PlayAnimFunc2 = EndMenuWidgetInstance->FindFunction(FName("TotalScoreAnim"));
		if (PlayAnimFunc2)
		{
			EndMenuWidgetInstance->ProcessEvent(PlayAnimFunc2, nullptr);
		}


		if (UTextBlock* TotalScoreText = Cast<UTextBlock>(EndMenuWidgetInstance->GetWidgetFromName(TEXT("TotalScoreText"))))
		{
			if (UProjectGameInstance* ProjecetGameInstance = Cast<UProjectGameInstance>(UGameplayStatics::GetGameInstance(this)))
			{
				TotalScoreText->SetText(FText::FromString(FString::Printf(TEXT("Total Score : %d "), ProjecetGameInstance->TotalScore)
				));
			}
		}
	}
}

void AProjectController::ShowGameHUD()
{
	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->RemoveFromParent();
		HUDWidgetInstance = nullptr;
	}

	if (MainMenuWidgetInstance)
	{
		MainMenuWidgetInstance->RemoveFromParent();
		MainMenuWidgetInstance = nullptr;
	}

	if (EndMenuWidgetInstance)
	{
		EndMenuWidgetInstance->RemoveFromParent();
		EndMenuWidgetInstance = nullptr;
	}

	if (HUDWidgetClass)
	{
		HUDWidgetInstance = CreateWidget<UUserWidget>(this, HUDWidgetClass);
		if (HUDWidgetInstance)
		{
			HUDWidgetInstance->AddToViewport();

			bShowMouseCursor = false;
			SetInputMode(FInputModeGameOnly());

		}

		AProjectGameState* ProjectGameState = GetWorld() ? GetWorld()->GetGameState<AProjectGameState>() : nullptr;
		if (ProjectGameState)
		{
			ProjectGameState->UpdateHUD();
		}
	}
}



void AProjectController::StartGame()
{
	if (UProjectGameInstance* ProjectGameInstance = Cast<UProjectGameInstance>(UGameplayStatics::GetGameInstance(this)) )
	{
		ProjectGameInstance->CurrentLevelIndex = 0;
		ProjectGameInstance->TotalScore = 0;
	}

	UGameplayStatics::OpenLevel(GetWorld(), FName("BasicLevel"));
	SetPause(false);
}

void AProjectController::EndGame()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	UKismetSystemLibrary::QuitGame(this, PlayerController, EQuitPreference::Quit, false);
}
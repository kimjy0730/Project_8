#include "ProjectCharacter.h"
#include "ProjectController.h"
#include "ProjectGameState.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/TextBlock.h"


AProjectCharacter::AProjectCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = 300.f;
	SpringArmComp->bUsePawnControlRotation = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false;

	OverHeadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverHeadWidget"));
	OverHeadWidget->SetupAttachment(GetMesh());
	OverHeadWidget->SetWidgetSpace(EWidgetSpace::Screen);

	NormalSpeed = 600.f;
	SprintSpeedMultiplier = 1.7f;
	SprintSpeed = NormalSpeed * SprintSpeedMultiplier;

	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
	
	MaxHealth = 100.f;
	Health = MaxHealth;
}


void AProjectCharacter::BeginPlay()
{
	Super::BeginPlay();

	UpdateOverheadHP();
}

void AProjectCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast< UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (AProjectController* PlayerController = Cast<AProjectController>(GetController()))
		{		
			if (PlayerController->MoveAction)
			{
				EnhancedInput->BindAction
				(
					PlayerController->MoveAction,
					ETriggerEvent::Triggered,
					this,
					&AProjectCharacter::Move
				);
			}
			if (PlayerController->JumpAction)
			{
				EnhancedInput->BindAction
				(
					PlayerController->JumpAction,
					ETriggerEvent::Triggered,
					this,
					&AProjectCharacter::StartJump
				);
				EnhancedInput->BindAction
				(
					PlayerController->JumpAction,
					ETriggerEvent::Completed,
					this,
					&AProjectCharacter::StopJump
				);
			}
			if (PlayerController->SprintAction)
			{
				EnhancedInput->BindAction
				(
					PlayerController->SprintAction,
					ETriggerEvent::Triggered,
					this,
					&AProjectCharacter::StartSprint
				);
				EnhancedInput->BindAction
				(
					PlayerController->SprintAction,
					ETriggerEvent::Completed,
					this,
					&AProjectCharacter::StopSprint
				);
			}
			if (PlayerController->LookAction)
			{
				EnhancedInput->BindAction
				(
					PlayerController->LookAction,
					ETriggerEvent::Triggered,
					this,
					&AProjectCharacter::Look
				);
			}
		}
	}
}


void AProjectCharacter::Move(const FInputActionValue& value)
{
	if (!Controller) return;

	const FVector2D MoveInput = value.Get<FVector2D>();

	if (!FMath::IsNearlyZero(MoveInput.X))
	{
		AddMovementInput(GetActorForwardVector(), MoveInput.X);
	}
	if (!FMath::IsNearlyZero(MoveInput.Y))
	{
		AddMovementInput(GetActorRightVector(), MoveInput.Y);
	}
}
void AProjectCharacter::StartJump(const FInputActionValue& value)
{
	if (value.Get<bool>())
	{
		Jump();
	}
}
void AProjectCharacter::StopJump(const FInputActionValue& value)
{
	if (!value.Get<bool>())
	{
		StopJumping();
	}
}
void AProjectCharacter::StartSprint(const FInputActionValue& value)
{
	if (value.Get<bool>())
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
	}
}
void AProjectCharacter::StopSprint(const FInputActionValue& value)
{
	if (!value.Get<bool>())
	{
		GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
	}
}
void AProjectCharacter::Look(const FInputActionValue& value)
{
	FVector2D LookInput = value.Get<FVector2D>();

	AddControllerYawInput(LookInput.X);
	AddControllerPitchInput(LookInput.Y);
}

float AProjectCharacter::GetHealth() const
{
	return Health;
}


void AProjectCharacter::AddHealth(float Amount)
{
	Health = FMath::Clamp(Health + Amount, 0.0f, MaxHealth);

	UpdateOverheadHP();
}

float AProjectCharacter::TakeDamage(
	float DamageAmount,
	struct FDamageEvent const& DamageEvent,
	AController* EventInstigator,
	AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(
		DamageAmount,
		DamageEvent,
		EventInstigator,
		DamageCauser);

	Health = FMath::Clamp(Health - DamageAmount, 0.0f, MaxHealth);

	UpdateOverheadHP();

	if (Health <= 0.f)
	{
		OnDeath();
	}

	return ActualDamage;
}

void AProjectCharacter::OnDeath()
{
	AProjectGameState* ProjectGameState = GetWorld() ? GetWorld()->GetGameState<AProjectGameState>() : nullptr;
	if (ProjectGameState)
	{
		ProjectGameState->OnGameOver();
	}
}

void AProjectCharacter::UpdateOverheadHP()
{
	if (!OverHeadWidget) return;

	UUserWidget* OverheadWidgetInstance = OverHeadWidget->GetUserWidgetObject();
	if (!OverheadWidgetInstance) return;

	if (UTextBlock* HPText = Cast<UTextBlock>(OverheadWidgetInstance->GetWidgetFromName(TEXT("OverHeadHP"))))
	{
		HPText->SetText(FText::FromString(FString::Printf(TEXT("%.0f / %.0f"), Health, MaxHealth)));
	}
}
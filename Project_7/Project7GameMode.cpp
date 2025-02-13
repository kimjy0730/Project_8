#include "Project7GameMode.h"
#include "ProjectCharacter.h"
#include "ProjectController.h"
#include "ProjectGameState.h"

AProject7GameMode::AProject7GameMode()
{
	DefaultPawnClass = AProjectCharacter::StaticClass();
	PlayerControllerClass = AProjectController::StaticClass();
	GameStateClass = AProjectGameState::StaticClass();
}
#include "CoinItem.h"
#include "Engine/World.h"
#include "ProjectGameState.h"

ACoinItem::ACoinItem()
{
	PointValue = 0;
	ItemType = "DefauteCoin";
}

void ACoinItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);

	if (Activator && Activator->ActorHasTag("Player"))
	{
		if (UWorld* World = GetWorld())
		{
			if (AProjectGameState* GameState = World->GetGameState< AProjectGameState>())
			{
				GameState->AddScore(PointValue);
				GameState->OnCoinCollected();
			}
		}
		
		DestroyItem();
	}
}
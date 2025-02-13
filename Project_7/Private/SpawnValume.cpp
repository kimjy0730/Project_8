#include "SpawnValume.h"
#include "Components/BoxComponent.h"


ASpawnValume::ASpawnValume()
{
 	
	PrimaryActorTick.bCanEverTick = false;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);

	SpawningBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawningBox"));
	SpawningBox-> SetupAttachment(Scene);

	ItemTable = nullptr;
}


FVector ASpawnValume::GetRandomPointInVolume() const
{
	FVector BoxExtent = SpawningBox->GetScaledBoxExtent();
	FVector BoxOrigin = SpawningBox->GetComponentLocation();

	return BoxOrigin + FVector(
		FMath::FRandRange(-BoxExtent.X, BoxExtent.X),
		FMath::FRandRange(-BoxExtent.Y, BoxExtent.Y),
		FMath::FRandRange(-BoxExtent.Z, BoxExtent.Z)
	);
}

AActor* ASpawnValume::SpawnRandomItem()
{
	if (FItemSpawnRow* SelectedRow = GetRandomItem())
	{
		if (UClass* ActualClass = SelectedRow->ItemClass.Get())
		{
			return SpawnItem(ActualClass);
		}
	}

	return nullptr;
}

FItemSpawnRow* ASpawnValume::GetRandomItem() const
{
	if (!ItemTable) return nullptr;
	
	TArray<FItemSpawnRow*> AllRows;
	static const FString ContextString(TEXT("ItemSpawnConText"));
	ItemTable->GetAllRows(ContextString, AllRows);

	if (AllRows.IsEmpty()) return nullptr;

	float TotalChance = 0.0f;

	for (const FItemSpawnRow* Row : AllRows)
	{
		TotalChance += Row->Spawnchance;
	}

	const float RandValue = FMath::FRandRange(0.0f, TotalChance);
	
	float AccumulatedChance = 0.f;

	for (FItemSpawnRow* Row : AllRows)
	{
		AccumulatedChance += Row->Spawnchance;
		if (AccumulatedChance >= RandValue)
		{
			return Row;
		}
	}

	return nullptr;
}

AActor* ASpawnValume::SpawnItem(TSubclassOf<AActor> ItemClass)
{
	if (!ItemClass) return nullptr;
	
	AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(
	ItemClass,
	GetRandomPointInVolume(),
	FRotator::ZeroRotator
	);


	return SpawnedActor;
}

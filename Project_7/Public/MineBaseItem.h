#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "MineBaseItem.generated.h"

UCLASS()
class PROJECT_7_API AMineBaseItem : public ABaseItem
{
	GENERATED_BODY()

public:
	AMineBaseItem();

	USphereComponent* ExplosionSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MineItem")
	float ExplosionDelay;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MineItem")
	float ExplosionRadius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MineItem")
	int32 DamageAmount;


	bool bHasExploded;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Effects")
	UParticleSystem* ExplosionParticle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Effects")
	USoundBase* ExplosionSound;

	FTimerHandle ExplosionTimer;

	virtual void ActivateItem(AActor* Activator) override;
	
	void Explode();


};

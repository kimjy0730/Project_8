#include "MineBaseItem.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

AMineBaseItem::AMineBaseItem()
{
	

	ExplosionDelay = 0.5f;
	ExplosionRadius = 400.0f;
	DamageAmount = 30;

	ItemType = "Mine Item";

	bHasExploded = false;

	ExplosionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("ExplosionCollision"));
	ExplosionSphere->InitSphereRadius(ExplosionRadius);
	ExplosionSphere->SetCollisionProfileName(TEXT("overlapAllDynamic"));
	ExplosionSphere->SetupAttachment(Scene);
}



void AMineBaseItem::ActivateItem(AActor* Activator)
{
	if (bHasExploded) return;

	Super::ActivateItem(Activator);

	GetWorld()->GetTimerManager().SetTimer(
		ExplosionTimer,
		this,
		&AMineBaseItem::Explode,
		ExplosionDelay,
		false
	);

	bHasExploded = true;
}

void AMineBaseItem::Explode()
{
	UParticleSystemComponent* Particle = nullptr;
	if (ExplosionParticle)
	{
		Particle = UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			ExplosionParticle,
			GetActorLocation(),
			GetActorRotation(),
			false
		);
	}
	
	if (ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			ExplosionSound,
			GetActorLocation()
		);
	}

	TArray<AActor*> OverlappingActors;
	ExplosionSphere->GetOverlappingActors(OverlappingActors);
	for (AActor* Actor : OverlappingActors)
	{
		if (Actor && Actor->ActorHasTag("Player"))
		{
			UGameplayStatics::ApplyDamage(
				Actor,
				DamageAmount,
				nullptr,
				this,
				UDamageType::StaticClass()
			);
		}
	}

	DestroyItem();

	if (Particle)
	{
		FTimerHandle DestroyParticleTimerHandle;

		GetWorld()->GetTimerManager().SetTimer(
			DestroyParticleTimerHandle,
			[Particle]()
			{
				if (Particle && Particle->IsValidLowLevel())
				{
					Particle->DestroyComponent();
				}
			},
			2.0f,
			false
		);
	}
}
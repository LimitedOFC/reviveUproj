// Copyright FNGameProj. All Rights Reserved.

#include "Weapons/FortWeaponComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "CollisionQueryParams.h"
#include "DrawDebugHelpers.h"
#include "AthenaCharacter.h"
#include "Camera/CameraComponent.h"

UFortWeaponComponent::UFortWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicated(true);
}

void UFortWeaponComponent::BeginPlay() { Super::BeginPlay(); }

void UFortWeaponComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UFortWeaponComponent, CurrentAmmo);
	DOREPLIFETIME(UFortWeaponComponent, bIsReloading);
	DOREPLIFETIME(UFortWeaponComponent, bIsADS);
}

void UFortWeaponComponent::EquipWeaponDef(UFortWeaponItemDefinition* InDef)
{
	GetWorld()->GetTimerManager().ClearTimer(ReloadTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(FireRateTimerHandle);
	bIsReloading = false;
	bIsFiring    = false;

	CurrentWeaponDef = InDef;
	CurrentAmmo = InDef ? InDef->MagazineSize : 0;
	OnAmmoChanged.Broadcast(CurrentAmmo, InDef ? InDef->MagazineSize : 0);
}

void UFortWeaponComponent::StartFire()
{
	if (!CurrentWeaponDef || bIsReloading || bIsFiring) { return; }

	if (CurrentAmmo <= 0)
	{
		Reload();
		return;
	}

	bIsFiring = true;
	ExecuteShot();

	if (CurrentWeaponDef->bIsAutomatic)
	{
		const float FireInterval = 1.0f / FMath::Max(CurrentWeaponDef->FireRate, 0.01f);
		GetWorld()->GetTimerManager().SetTimer(FireRateTimerHandle, this,
			&UFortWeaponComponent::DoFullAutoFire, FireInterval, true);
	}
}

void UFortWeaponComponent::StopFire()
{
	bIsFiring = false;
	GetWorld()->GetTimerManager().ClearTimer(FireRateTimerHandle);
}

void UFortWeaponComponent::DoFullAutoFire()
{
	if (!bIsFiring || !CurrentWeaponDef || CurrentAmmo <= 0)
	{
		StopFire();
		if (CurrentAmmo <= 0) { Reload(); }
		return;
	}
	ExecuteShot();
}

void UFortWeaponComponent::ExecuteShot()
{
	if (!CurrentWeaponDef || CurrentAmmo <= 0) { return; }

	--CurrentAmmo;
	OnAmmoChanged.Broadcast(CurrentAmmo, CurrentWeaponDef->MagazineSize);
	OnWeaponFired.Broadcast();

	AAthenaCharacter* Owner = Cast<AAthenaCharacter>(GetOwner());
	if (!Owner) { return; }

	// Line trace (hitscan)
	const int32 Pellets = FMath::Max(CurrentWeaponDef->PelletsPerShot, 1);
	for (int32 p = 0; p < Pellets; ++p)
	{
		FVector Start   = Owner->GetFollowCamera()->GetComponentLocation();
		FVector Forward = Owner->GetFollowCamera()->GetForwardVector();

		// Apply spread
		float SpreadAngle = bIsADS ? CurrentWeaponDef->ADSSpread : CurrentWeaponDef->BulletSpread;
		FVector SpreadDir = FMath::VRandCone(Forward, FMath::DegreesToRadians(SpreadAngle));
		FVector End = Start + SpreadDir * CurrentWeaponDef->EffectiveRange;

		FHitResult Hit;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(Owner);

		if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
		{
			if (AActor* HitActor = Hit.GetActor())
			{
				bool bHeadshot = Hit.BoneName == FName("Head");
				float Damage = CurrentWeaponDef->DamagePerShot
				             * (bHeadshot ? CurrentWeaponDef->HeadshotMultiplier : 1.0f);

				FPointDamageEvent DmgEvent(Damage, Hit, SpreadDir, nullptr);
				HitActor->TakeDamage(Damage, DmgEvent, Owner->GetController(), Owner);
			}
		}
	}

	if (CurrentAmmo <= 0 && !bIsReloading)
	{
		Reload();
	}
}

void UFortWeaponComponent::Reload()
{
	if (!CurrentWeaponDef || bIsReloading ||
	    CurrentAmmo >= CurrentWeaponDef->MagazineSize) { return; }

	bIsReloading = true;
	GetWorld()->GetTimerManager().SetTimer(
		ReloadTimerHandle, this, &UFortWeaponComponent::FinishReload,
		CurrentWeaponDef->ReloadTime, false);
}

void UFortWeaponComponent::FinishReload()
{
	bIsReloading = false;
	if (CurrentWeaponDef)
	{
		CurrentAmmo = CurrentWeaponDef->MagazineSize;
		OnAmmoChanged.Broadcast(CurrentAmmo, CurrentWeaponDef->MagazineSize);
		OnWeaponReloaded.Broadcast();
	}
}

void UFortWeaponComponent::SetADS(bool bAiming)
{
	bIsADS = bAiming;
}

void UFortWeaponComponent::OnRep_CurrentAmmo()
{
	if (CurrentWeaponDef)
	{
		OnAmmoChanged.Broadcast(CurrentAmmo, CurrentWeaponDef->MagazineSize);
	}
}

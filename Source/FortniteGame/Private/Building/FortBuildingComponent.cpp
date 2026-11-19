// Copyright FNGameProj. All Rights Reserved.

#include "Building/FortBuildingComponent.h"
#include "Building/FortBuildingSettings.h"
#include "Inventory/FortInventoryComponent.h"
#include "AthenaCharacter.h"
#include "Engine/World.h"
#include "CollisionQueryParams.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

// ─────────────────────────────────────────────────────────────────────────────
// Construction
// ─────────────────────────────────────────────────────────────────────────────

UFortBuildingComponent::UFortBuildingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

// ─────────────────────────────────────────────────────────────────────────────
// BeginPlay — pull settings from config immediately
// ─────────────────────────────────────────────────────────────────────────────

void UFortBuildingComponent::BeginPlay()
{
	Super::BeginPlay();
	RefreshSettingsFromConfig();
}

// ─────────────────────────────────────────────────────────────────────────────
// Tick — ghost preview update
// ─────────────────────────────────────────────────────────────────────────────

void UFortBuildingComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                            FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// Ghost placement preview position updated every tick (Blueprint handles visual)
}

// ─────────────────────────────────────────────────────────────────────────────
// Settings application
// ─────────────────────────────────────────────────────────────────────────────

void UFortBuildingComponent::RefreshSettingsFromConfig()
{
	const UFortBuildingSettings* S = GetDefault<UFortBuildingSettings>();
	if (S) { ApplyBuildingSettings(S); }
}

void UFortBuildingComponent::ApplyBuildingSettings(const UFortBuildingSettings* S)
{
	if (!ensureMsgf(S, TEXT("ApplyBuildingSettings received a null settings object."))) { return; }

	bOpt_SimpleEdit           = S->bSimpleEdit;
	bOpt_EditOnRelease        = S->bEditOnRelease;
	bOpt_ConfirmEditOnRelease = S->bConfirmEditOnRelease;
	bOpt_ResetOnRelease       = S->bResetOnRelease;
	bOpt_DisablePreEdits      = S->bDisablePreEdits;
	bOpt_Turbobuild           = S->bTurbobuild;
	bOpt_BuilderProSupport    = S->bBuilderProSupport;
	bOpt_InstantEditToggle    = S->bInstantEditToggle;
	fOpt_EditHoldTime         = S->GetEffectiveEditHoldTime();
	fOpt_EditResetDelay       = S->EditResetDelay;

	// If pre-edits are disabled, clear any carried-over last preset
	if (bOpt_DisablePreEdits)
	{
		LastConfirmedPreset = EFortEditPreset::None;
	}

	OnSettingsApplied.Broadcast(S);
}

// ─────────────────────────────────────────────────────────────────────────────
// Build mode
// ─────────────────────────────────────────────────────────────────────────────

void UFortBuildingComponent::SetBuildModeActive(bool bActive)
{
	if (bBuildModeActive == bActive) { return; }

	bBuildModeActive = bActive;

	if (bActive)
	{
		SetComponentTickEnabled(true);
		OnBuildModeEnabled.Broadcast();
		// Exit edit mode if we were in it
		if (EditState != EFortEditState::Idle) { CloseEditCursor(); }
	}
	else
	{
		SetComponentTickEnabled(false);
		StopTurbobuild();
		OnBuildModeDisabled.Broadcast();
	}
}

void UFortBuildingComponent::SetActivePieceType(EFortBuildPieceType PieceType)
{
	ActivePieceType = PieceType;
}

void UFortBuildingComponent::SetActiveMaterial(EFortBuildMaterial Material)
{
	ActiveMaterial = Material;
}

bool UFortBuildingComponent::TryPlacePiece()
{
	if (!bBuildModeActive) { return false; }

	AAthenaCharacter* Owner = Cast<AAthenaCharacter>(GetOwner());
	if (!Owner) { return false; }

	UFortInventoryComponent* Inv = Owner->FindComponentByClass<UFortInventoryComponent>();
	if (!Inv) { return false; }

	const int32 Cost = FFortBuildCost::Get(ActivePieceType, ActiveMaterial);
	if (!Inv->ConsumeResource(ActiveMaterial, Cost)) { return false; }

	// Actual piece spawn — Blueprint child class handles visual / actor spawn
	OnPiecePlaced.Broadcast(ActivePieceType);
	return true;
}

void UFortBuildingComponent::RotatePlacement()
{
	CurrentRotationYaw = FMath::Fmod(CurrentRotationYaw + 90.0f, 360.0f);
}

// ─────────────────────────────────────────────────────────────────────────────
// Edit key input — entry points (called from player input component)
// ─────────────────────────────────────────────────────────────────────────────

void UFortBuildingComponent::EditKeyPressed()
{
	// Already in edit — ignore re-press
	if (EditState == EFortEditState::Open) { return; }

	// Simple Edit or Instant Edit Toggle: open immediately, no hold threshold
	if (bOpt_SimpleEdit || bOpt_InstantEditToggle || !bOpt_BuilderProSupport)
	{
		OpenEditCursor();
		return;
	}

	// Builder Pro with hold threshold
	if (fOpt_EditHoldTime > 0.0f)
	{
		EditState = EFortEditState::Holding;
		GetWorld()->GetTimerManager().SetTimer(
			EditHoldTimerHandle,
			this,
			&UFortBuildingComponent::OnEditHoldThresholdMet,
			fOpt_EditHoldTime,
			false);
	}
	else
	{
		OpenEditCursor();
	}
}

void UFortBuildingComponent::EditKeyReleased()
{
	// If we were waiting for the hold threshold and the key was released early — cancel
	if (EditState == EFortEditState::Holding)
	{
		GetWorld()->GetTimerManager().ClearTimer(EditHoldTimerHandle);
		EditState = EFortEditState::Idle;
		return;
	}

	if (EditState != EFortEditState::Open) { return; }

	// Simple Edit or Edit-On-Release: release = confirm (or reset if nothing selected)
	if (bOpt_SimpleEdit || bOpt_EditOnRelease)
	{
		if (HasCellSelected())
		{
			ExecuteConfirm();
		}
		else if (bOpt_ResetOnRelease && ResetDelayElapsed())
		{
			ExecuteReset();
		}
		else
		{
			CloseEditCursor();
		}
		return;
	}

	// Reset-On-Release with no selection
	if (bOpt_ResetOnRelease && !HasCellSelected() && ResetDelayElapsed())
	{
		ExecuteReset();
		return;
	}

	// Standard: release does nothing — edit stays open until confirm/reset click
}

// ─────────────────────────────────────────────────────────────────────────────
// Confirm edit key (Builder Pro has this on a separate bind)
// ─────────────────────────────────────────────────────────────────────────────

void UFortBuildingComponent::ConfirmEditKeyPressed()
{
	if (EditState != EFortEditState::Open) { return; }

	// If Confirm-On-Release is active, mark as "confirming" and wait for release
	if (bOpt_ConfirmEditOnRelease || bOpt_SimpleEdit)
	{
		EditState = EFortEditState::Confirming;
	}
	else
	{
		ExecuteConfirm();
	}
}

void UFortBuildingComponent::ConfirmEditKeyReleased()
{
	if (EditState == EFortEditState::Confirming)
	{
		ExecuteConfirm();
	}
}

// ─────────────────────────────────────────────────────────────────────────────
// Programmatic edit API
// ─────────────────────────────────────────────────────────────────────────────

void UFortBuildingComponent::BeginEdit()
{
	OpenEditCursor();
}

void UFortBuildingComponent::ConfirmEdit()
{
	if (EditState == EFortEditState::Idle) { return; }
	ExecuteConfirm();
}

void UFortBuildingComponent::ResetEdit()
{
	if (EditState == EFortEditState::Idle) { return; }
	ExecuteReset();
}

void UFortBuildingComponent::ApplyEditPreset(EFortEditPreset Preset)
{
	PendingPreset = Preset;
	// Blueprint handles the visual cell-toggle representation of this preset.
	// Calling ConfirmEdit() after this applies it to the piece.
}

// ─────────────────────────────────────────────────────────────────────────────
// Turbobuild
// ─────────────────────────────────────────────────────────────────────────────

void UFortBuildingComponent::StartTurbobuild()
{
	if (!bOpt_Turbobuild || !bBuildModeActive || bTurbobuilding) { return; }

	bTurbobuilding = true;
	// Place one piece immediately on press, then repeat on the interval timer
	TryPlacePiece();
	GetWorld()->GetTimerManager().SetTimer(
		TurbobuildTimerHandle,
		this,
		&UFortBuildingComponent::DoTurbobuildTick,
		TurbobuildInterval,
		true);
}

void UFortBuildingComponent::StopTurbobuild()
{
	bTurbobuilding = false;
	GetWorld()->GetTimerManager().ClearTimer(TurbobuildTimerHandle);
}

void UFortBuildingComponent::DoTurbobuildTick()
{
	if (!bTurbobuilding || !bBuildModeActive)
	{
		StopTurbobuild();
		return;
	}
	TryPlacePiece();
}

// ─────────────────────────────────────────────────────────────────────────────
// Internal helpers
// ─────────────────────────────────────────────────────────────────────────────

void UFortBuildingComponent::OpenEditCursor()
{
	bBuildModeActive = false; // edit mode is mutually exclusive with build placement
	EditState        = EFortEditState::Open;

	// Pre-edit: restore last confirmed shape unless DisablePreEdits is set
	if (!bOpt_DisablePreEdits && LastConfirmedPreset != EFortEditPreset::None)
	{
		PendingPreset = LastConfirmedPreset;
	}
	else
	{
		PendingPreset = EFortEditPreset::None;
	}

	OnEditOpened.Broadcast();
}

void UFortBuildingComponent::CloseEditCursor()
{
	EditState = EFortEditState::Idle;
}

void UFortBuildingComponent::ExecuteConfirm()
{
	const EFortEditPreset Applied = PendingPreset;
	LastConfirmedPreset = Applied;
	LastConfirmTime     = GetWorld() ? (float)GetWorld()->GetTimeSeconds() : 0.0f;

	CloseEditCursor();
	OnEditConfirmed.Broadcast(Applied);
}

void UFortBuildingComponent::ExecuteReset()
{
	PendingPreset       = EFortEditPreset::None;
	LastConfirmedPreset = EFortEditPreset::None;

	CloseEditCursor();
	OnEditResetFired.Broadcast();
}

bool UFortBuildingComponent::HasCellSelected() const
{
	// PendingPreset == None means no cell was selected yet (cursor is on "full" shape)
	return PendingPreset != EFortEditPreset::None;
}

bool UFortBuildingComponent::ResetDelayElapsed() const
{
	if (!GetWorld()) { return true; }
	const float Now = (float)GetWorld()->GetTimeSeconds();
	return (Now - LastConfirmTime) >= fOpt_EditResetDelay;
}

void UFortBuildingComponent::OnEditHoldThresholdMet()
{
	// Only open if the player is still holding (state == Holding)
	if (EditState == EFortEditState::Holding)
	{
		OpenEditCursor();
	}
}

FVector UFortBuildingComponent::GetSnappedPlacementLocation() const
{
	if (!GetOwner()) { return FVector::ZeroVector; }

	const FVector Loc    = GetOwner()->GetActorLocation();
	const FVector Fwd    = GetOwner()->GetActorForwardVector();
	FVector Raw          = Loc + Fwd * (PlacementTraceLength * 0.75f);

	Raw.X = FMath::RoundToFloat(Raw.X / BuildGridSize) * BuildGridSize;
	Raw.Y = FMath::RoundToFloat(Raw.Y / BuildGridSize) * BuildGridSize;
	Raw.Z = FMath::RoundToFloat(Raw.Z / BuildGridSize) * BuildGridSize;
	return Raw;
}

AActor* UFortBuildingComponent::FindBuildTarget() const
{
	if (!GetOwner()) { return nullptr; }

	const FVector Start = GetOwner()->GetActorLocation();
	const FVector End   = Start + GetOwner()->GetActorForwardVector() * PlacementTraceLength;
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());

	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_WorldDynamic, Params))
	{
		return Hit.GetActor();
	}
	return nullptr;
}

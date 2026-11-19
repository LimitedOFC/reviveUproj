// Copyright FNGameProj. All Rights Reserved.

#include "AthenaCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"
#include "Inventory/FortInventoryComponent.h"
#include "Building/FortBuildingComponent.h"
#include "Building/FortBuildingSettings.h"
#include "Building/SFortBuildingSettingsMenu.h"
#include "Weapons/FortWeaponComponent.h"
#include "Cosmetics/FortCosmeticManager.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

AAthenaCharacter::AAthenaCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	bAlwaysRelevant = false;
	NetUpdateFrequency = 100.0f;
	MinNetUpdateFrequency = 33.0f;

	// Camera
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraLagSpeed = 10.0f;
	CameraBoom->bEnableCameraRotationLag = false;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Gameplay systems
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystem"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	InventoryComponent = CreateDefaultSubobject<UFortInventoryComponent>(TEXT("Inventory"));
	BuildingComponent  = CreateDefaultSubobject<UFortBuildingComponent>(TEXT("Building"));
	WeaponComponent    = CreateDefaultSubobject<UFortWeaponComponent>(TEXT("Weapon"));

	// Back bling socket
	BackBlingMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BackBlingMesh"));
	BackBlingMesh->SetupAttachment(GetMesh(), FName("BackBlingSocket"));
	BackBlingMesh->bCastDynamicShadow = true;

	// S9 movement defaults
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->MaxWalkSpeed = S9_WalkSpeed;
	GetCharacterMovement()->MaxWalkSpeedCrouched = S9_CrouchSpeed;
	GetCharacterMovement()->JumpZVelocity = S9_JumpVelocity;
	GetCharacterMovement()->AirControl = 0.3f;
	GetCharacterMovement()->GravityScale = 1.75f;
	GetCharacterMovement()->MaxFlySpeed = 800.0f;
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCharacterMovement()->NavAgentProps.bCanSwim = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
}

// ── IAbilitySystemInterface ───────────────────────────────────────────────────

UAbilitySystemComponent* AAthenaCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

// ── Lifetime replication ──────────────────────────────────────────────────────

void AAthenaCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAthenaCharacter, CurrentHealth);
	DOREPLIFETIME(AAthenaCharacter, CurrentShield);
	DOREPLIFETIME(AAthenaCharacter, MaxHealth);
	DOREPLIFETIME(AAthenaCharacter, MaxShield);
	DOREPLIFETIME(AAthenaCharacter, bIsDowned);
	DOREPLIFETIME(AAthenaCharacter, CurrentLoadout);
}

// ── BeginPlay / Tick ──────────────────────────────────────────────────────────

void AAthenaCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		// Initialize attribute values
		CurrentHealth = MaxHealth;
		CurrentShield = 0.0f;
	}
}

void AAthenaCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (BuildingSettingsMenu.IsValid())
	{
		if (GEngine && GEngine->GameViewport)
		{
			GEngine->GameViewport->RemoveViewportWidgetContent(BuildingSettingsMenu.ToSharedRef());
		}
		BuildingSettingsMenu.Reset();
	}

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		PlayerController->bShowMouseCursor = false;
		PlayerController->SetInputMode(FInputModeGameOnly());
	}

	Super::EndPlay(EndPlayReason);
}

void AAthenaCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

// ── Input ─────────────────────────────────────────────────────────────────────

void AAthenaCharacter::SetupPlayerInputComponent(UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

	InputComponent->BindAction("Jump",           IE_Pressed,  this, &ACharacter::Jump);
	InputComponent->BindAction("Jump",           IE_Released, this, &ACharacter::StopJumping);
	InputComponent->BindAction("Sprint",         IE_Pressed,  this, &AAthenaCharacter::StartSprint);
	InputComponent->BindAction("Sprint",         IE_Released, this, &AAthenaCharacter::StopSprint);
	InputComponent->BindAction("Crouch",         IE_Pressed,  this, &AAthenaCharacter::StartCrouch);
	InputComponent->BindAction("Crouch",         IE_Released, this, &AAthenaCharacter::StopCrouch);
	InputComponent->BindAction("Fire",           IE_Pressed,  this, &AAthenaCharacter::StartFire);
	InputComponent->BindAction("Fire",           IE_Released, this, &AAthenaCharacter::StopFire);
	InputComponent->BindAction("Reload",         IE_Pressed,  this, &AAthenaCharacter::Reload);
	InputComponent->BindAction("ToggleBuildMode",IE_Pressed,  this, &AAthenaCharacter::ToggleBuildMode);
	InputComponent->BindAction("EditBuilding",   IE_Pressed,  this, &AAthenaCharacter::StartEditBuilding);
	InputComponent->BindAction("EditBuilding",   IE_Released, this, &AAthenaCharacter::StopEditBuilding);
	InputComponent->BindAction("ConfirmEdit",    IE_Pressed,  this, &AAthenaCharacter::ConfirmEditPressed);
	InputComponent->BindAction("ConfirmEdit",    IE_Released, this, &AAthenaCharacter::ConfirmEditReleased);
	InputComponent->BindAction("ResetEdit",      IE_Pressed,  this, &AAthenaCharacter::ResetEdit);
	InputComponent->BindAction("ToggleBuildingSettings", IE_Pressed, this, &AAthenaCharacter::ToggleBuildingSettings);

	InputComponent->BindAxis("MoveForward", this, &AAthenaCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AAthenaCharacter::MoveRight);

	InputComponent->BindAxis("Turn",    this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("LookUp",  this, &APawn::AddControllerPitchInput);

	InputComponent->BindAxis("ScrollSlot", this, &AAthenaCharacter::ScrollInventory);
}

// ── Movement ──────────────────────────────────────────────────────────────────

void AAthenaCharacter::MoveForward(float Value)
{
	if (Controller && Value != 0.0f)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FVector Direction = FRotationMatrix(FRotator(0, Rotation.Yaw, 0)).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AAthenaCharacter::MoveRight(float Value)
{
	if (Controller && Value != 0.0f)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FVector Direction = FRotationMatrix(FRotator(0, Rotation.Yaw, 0)).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

void AAthenaCharacter::ScrollInventory(float Value)
{
	if (FMath::Abs(Value) > 0.1f)
	{
		CycleInventorySlot(Value > 0.0f ? 1 : -1);
	}
}

void AAthenaCharacter::StartSprint()
{
	bIsSprinting = true;
	GetCharacterMovement()->MaxWalkSpeed = S9_SprintSpeed;
	CurrentMovementMode = EAthenaMovementMode::Sprinting;
}

void AAthenaCharacter::StopSprint()
{
	bIsSprinting = false;
	GetCharacterMovement()->MaxWalkSpeed = S9_WalkSpeed;
	CurrentMovementMode = EAthenaMovementMode::Walking;
}

void AAthenaCharacter::StartCrouch()
{
	Crouch();
	CurrentMovementMode = EAthenaMovementMode::Crouching;
}

void AAthenaCharacter::StopCrouch()
{
	UnCrouch();
	CurrentMovementMode = bIsSprinting ? EAthenaMovementMode::Sprinting : EAthenaMovementMode::Walking;
}

void AAthenaCharacter::OpenGlider()
{
	CurrentMovementMode = EAthenaMovementMode::Gliding;
	GetCharacterMovement()->SetMovementMode(MOVE_Flying);
}

void AAthenaCharacter::StartSkydive()
{
	CurrentMovementMode = EAthenaMovementMode::Skydiving;
	GetCharacterMovement()->SetMovementMode(MOVE_Falling);
}

void AAthenaCharacter::UpdateMovementSpeed()
{
	if (bIsSprinting)
		GetCharacterMovement()->MaxWalkSpeed = S9_SprintSpeed;
	else
		GetCharacterMovement()->MaxWalkSpeed = S9_WalkSpeed;
}

// ── Damage / Health ───────────────────────────────────────────────────────────

float AAthenaCharacter::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent,
                                     AController* EventInstigator, AActor* DamageCauser)
{
	if (!HasAuthority() || !IsAlive() || bIsDowned) { return 0.0f; }

	float Remaining = DamageAmount;

	// Shields absorb first
	if (CurrentShield > 0.0f)
	{
		float ShieldAbsorb = FMath::Min(CurrentShield, Remaining);
		ApplyShieldChange(-ShieldAbsorb, DamageCauser);
		Remaining -= ShieldAbsorb;
	}

	if (Remaining > 0.0f)
	{
		ApplyHealthChange(-Remaining, DamageCauser);
	}

	return DamageAmount;
}

void AAthenaCharacter::ApplyHealthChange(float Delta, AActor* DamageSource)
{
	if (!HasAuthority()) { return; }

	float OldHealth = CurrentHealth;
	CurrentHealth = FMath::Clamp(CurrentHealth + Delta, 0.0f, MaxHealth);

	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);

	if (CurrentHealth <= 0.0f && OldHealth > 0.0f)
	{
		HandleElimination(DamageSource);
	}
}

void AAthenaCharacter::ApplyShieldChange(float Delta, AActor* DamageSource)
{
	if (!HasAuthority()) { return; }

	CurrentShield = FMath::Clamp(CurrentShield + Delta, 0.0f, MaxShield);
	OnShieldChanged.Broadcast(CurrentShield, MaxShield);
}

void AAthenaCharacter::HandleElimination(AActor* Eliminator)
{
	bIsDowned = true;
	CurrentMovementMode = EAthenaMovementMode::Downed;

	AAthenaCharacter* EliminatorChar = Cast<AAthenaCharacter>(Eliminator);
	OnPlayerEliminated.Broadcast(this, EliminatorChar);

	GetCharacterMovement()->DisableMovement();
	SetActorEnableCollision(false);

	// Destroy after brief delay (allow elimination animation to play)
	SetLifeSpan(5.0f);
}

// ── Building ──────────────────────────────────────────────────────────────────

void AAthenaCharacter::ToggleBuildMode()
{
	bIsInBuildMode = !bIsInBuildMode;
	if (BuildingComponent)
	{
		BuildingComponent->SetBuildModeActive(bIsInBuildMode);
	}
}

void AAthenaCharacter::PlaceBuildPiece()
{
	if (bIsInBuildMode && BuildingComponent)
	{
		BuildingComponent->TryPlacePiece();
	}
}

void AAthenaCharacter::StartEditBuilding()
{
	if (BuildingComponent)
	{
		BuildingComponent->EditKeyPressed();
	}
}

void AAthenaCharacter::StopEditBuilding()
{
	if (BuildingComponent)
	{
		BuildingComponent->EditKeyReleased();
	}
}

void AAthenaCharacter::ConfirmEditPressed()
{
	if (BuildingComponent)
	{
		BuildingComponent->ConfirmEditKeyPressed();
	}
}

void AAthenaCharacter::ConfirmEditReleased()
{
	if (BuildingComponent)
	{
		BuildingComponent->ConfirmEditKeyReleased();
	}
}

void AAthenaCharacter::ConfirmEdit()
{
	if (BuildingComponent) { BuildingComponent->ConfirmEdit(); }
}

void AAthenaCharacter::ResetEdit()
{
	if (BuildingComponent) { BuildingComponent->ResetEdit(); }
}

void AAthenaCharacter::ToggleBuildingSettings()
{
	if (BuildingSettingsMenu.IsValid())
	{
		if (GEngine && GEngine->GameViewport)
		{
			GEngine->GameViewport->RemoveViewportWidgetContent(BuildingSettingsMenu.ToSharedRef());
		}
		BuildingSettingsMenu.Reset();
		if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
		{
			PlayerController->bShowMouseCursor = false;
			PlayerController->SetInputMode(FInputModeGameOnly());
		}
		return;
	}

	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	if (!GEngine || !GEngine->GameViewport || !PlayerController)
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot open building settings without a local player controller and game viewport."));
		return;
	}

	TWeakObjectPtr<AAthenaCharacter> WeakThis(this);
	TSharedRef<SFortBuildingSettingsMenu> Menu = SNew(SFortBuildingSettingsMenu)
		.Settings(GetMutableDefault<UFortBuildingSettings>())
		.BuildingComponent(BuildingComponent)
		.PlayerController(PlayerController)
		.OnClosed(FSimpleDelegate::CreateLambda([WeakThis]()
		{
			if (WeakThis.IsValid())
			{
				WeakThis->BuildingSettingsMenu.Reset();
			}
		}));

	BuildingSettingsMenu = Menu;
	GEngine->GameViewport->AddViewportWidgetContent(Menu, 100);
	PlayerController->bShowMouseCursor = true;
	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(Menu);
	PlayerController->SetInputMode(InputMode);
}

// ── Weapon ────────────────────────────────────────────────────────────────────

void AAthenaCharacter::StartFire()
{
	if (BuildingComponent && BuildingComponent->IsBuildModeActive())
	{
		BuildingComponent->StartTurbobuild();
		return;
	}

	if (BuildingComponent && BuildingComponent->IsEditModeActive())
	{
		return;
	}

	if (WeaponComponent) { WeaponComponent->StartFire(); }
}

void AAthenaCharacter::StopFire()
{
	if (BuildingComponent && BuildingComponent->IsTurbobuilding())
	{
		BuildingComponent->StopTurbobuild();
	}
	if (WeaponComponent) { WeaponComponent->StopFire(); }
}

void AAthenaCharacter::Reload()
{
	if (WeaponComponent) { WeaponComponent->Reload(); }
}

void AAthenaCharacter::ADS(bool bAiming)
{
	if (WeaponComponent) { WeaponComponent->SetADS(bAiming); }
}

// ── Inventory ─────────────────────────────────────────────────────────────────

void AAthenaCharacter::PickupItem(UFortWorldItem* WorldItem)
{
	if (InventoryComponent) { InventoryComponent->TryPickup(WorldItem); }
}

void AAthenaCharacter::DropCurrentItem()
{
	if (InventoryComponent) { InventoryComponent->DropActiveSlot(GetActorLocation(), GetActorForwardVector()); }
}

void AAthenaCharacter::CycleInventorySlot(int32 Direction)
{
	if (InventoryComponent) { InventoryComponent->CycleSlot(Direction); }
}

void AAthenaCharacter::SelectInventorySlot(int32 SlotIndex)
{
	if (InventoryComponent) { InventoryComponent->SelectSlot(SlotIndex); }
}

void AAthenaCharacter::EquipWeapon(UFortWeaponItemDefinition* WeaponDef)
{
	if (WeaponComponent) { WeaponComponent->EquipWeaponDef(WeaponDef); }
}

// ── Cosmetics ─────────────────────────────────────────────────────────────────

void AAthenaCharacter::ApplyLoadout(const FFortPlayerLoadout& NewLoadout)
{
	CurrentLoadout = NewLoadout;

	if (UGameInstance* GI = GetGameInstance())
	{
		if (UFortCosmeticManager* CM = GI->GetSubsystem<UFortCosmeticManager>())
		{
			CM->ApplyOutfitToMesh(GetMesh(), NewLoadout.OutfitID);
			CM->ApplyBackBling(this, NewLoadout.BackBlingID);
		}
	}
}

// ── Rep notifies ──────────────────────────────────────────────────────────────

void AAthenaCharacter::OnRep_CurrentHealth()
{
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}

void AAthenaCharacter::OnRep_CurrentShield()
{
	OnShieldChanged.Broadcast(CurrentShield, MaxShield);
}

void AAthenaCharacter::OnRep_bIsDowned()
{
	if (bIsDowned)
	{
		CurrentMovementMode = EAthenaMovementMode::Downed;
		GetCharacterMovement()->DisableMovement();
	}
}

void AAthenaCharacter::OnRep_CurrentLoadout()
{
	ApplyLoadout(CurrentLoadout);
}

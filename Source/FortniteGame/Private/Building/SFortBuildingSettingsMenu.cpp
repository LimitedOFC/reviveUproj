#include "Building/SFortBuildingSettingsMenu.h"

#include "Building/FortBuildingComponent.h"
#include "Building/FortBuildingSettings.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "Framework/Application/SlateApplication.h"
#include "GameFramework/PlayerController.h"
#include "HAL/FileManager.h"
#include "Misc/Paths.h"
#include "Styling/CoreStyle.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SSeparator.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"

namespace
{
	const FLinearColor BackgroundColor(0.012f, 0.025f, 0.10f, 0.96f);
	const FLinearColor PanelColor(0.025f, 0.065f, 0.20f, 0.98f);
	const FLinearColor RowColor(0.035f, 0.09f, 0.26f, 1.0f);
	const FLinearColor AccentColor(0.08f, 0.82f, 1.0f, 1.0f);
	const FLinearColor TextColor(0.89f, 0.94f, 1.0f, 1.0f);

	FText FormatSeconds(float Value)
	{
		return FText::FromString(FString::Printf(TEXT("%.2f s"), Value));
	}
}

void SFortBuildingSettingsMenu::Construct(const FArguments& InArgs)
{
	Settings = InArgs._Settings;
	BuildingComponent = InArgs._BuildingComponent;
	PlayerController = InArgs._PlayerController;
	OnClosed = InArgs._OnClosed;

	const FString LogoPath = FPaths::Combine(
		FPaths::ProjectContentDir(),
		TEXT("UI/Brand/ProjectLogo.png"));

	TSharedRef<SWidget> LogoWidget = SNew(STextBlock)
		.Text(FText::FromString(TEXT("+")))
		.ColorAndOpacity(AccentColor);

	if (IFileManager::Get().FileExists(*LogoPath))
	{
		LogoBrush = MakeShareable(new FSlateImageBrush(LogoPath, FVector2D(64.0f, 64.0f)));
		LogoWidget = SNew(SImage).Image(LogoBrush.Get());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Building settings logo is missing: %s"), *LogoPath);
	}

	ChildSlot
	[
		SNew(SOverlay)
		+ SOverlay::Slot()
		[
			SNew(SBorder)
			.BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
			.BorderBackgroundColor(BackgroundColor)
		]
		+ SOverlay::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SBox)
			.WidthOverride(1120.0f)
			.HeightOverride(700.0f)
			[
				SNew(SBorder)
				.Padding(18.0f)
				.BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
				.BorderBackgroundColor(PanelColor)
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					.AutoHeight()
					.Padding(0.0f, 0.0f, 0.0f, 14.0f)
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.AutoWidth()
						.VAlign(VAlign_Center)
						.Padding(0.0f, 0.0f, 14.0f, 0.0f)
						[
							SNew(SBox)
							.WidthOverride(64.0f)
							.HeightOverride(64.0f)
							[
								LogoWidget
							]
						]
						+ SHorizontalBox::Slot()
						.FillWidth(1.0f)
						.VAlign(VAlign_Center)
						[
							SNew(SVerticalBox)
							+ SVerticalBox::Slot()
							.AutoHeight()
							[
								SNew(STextBlock)
								.Text(FText::FromString(TEXT("BUILDING SETTINGS")))
								.ColorAndOpacity(TextColor)
								.Font(FCoreStyle::Get().GetFontStyle("LargeFont"))
							]
							+ SVerticalBox::Slot()
							.AutoHeight()
							.Padding(0.0f, 4.0f, 0.0f, 0.0f)
							[
								SNew(STextBlock)
								.Text(FText::FromString(TEXT("Customize editing and building controls")))
								.ColorAndOpacity(AccentColor)
							]
						]
						+ SHorizontalBox::Slot()
						.AutoWidth()
						.VAlign(VAlign_Top)
						[
							SNew(SButton)
							.ButtonColorAndOpacity(FLinearColor(0.08f, 0.30f, 0.68f, 1.0f))
							.OnClicked(this, &SFortBuildingSettingsMenu::CloseMenu)
							[
								SNew(STextBlock)
								.Text(FText::FromString(TEXT("CLOSE  [ESC]")))
								.ColorAndOpacity(TextColor)
							]
						]
					]
					+ SVerticalBox::Slot()
					.AutoHeight()
					.Padding(0.0f, 0.0f, 0.0f, 12.0f)
					[
						SNew(SSeparator)
					]
					+ SVerticalBox::Slot()
					.FillHeight(1.0f)
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.FillWidth(0.62f)
						.Padding(0.0f, 0.0f, 10.0f, 0.0f)
						[
							SNew(SBorder)
							.Padding(10.0f)
							.BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
							.BorderBackgroundColor(FLinearColor(0.018f, 0.04f, 0.14f, 0.98f))
							[
								SNew(SScrollBox)
								+ SScrollBox::Slot()
								.Padding(2.0f)
								[
									SNew(SVerticalBox)
									+ SVerticalBox::Slot().AutoHeight().Padding(4.0f, 2.0f, 0.0f, 6.0f)
									[
										MakeSectionHeading(FText::FromString(TEXT("EDITING")))
									]
									+ SVerticalBox::Slot().AutoHeight().Padding(0.0f, 0.0f, 0.0f, 6.0f)
									[
										MakeToggleRow(
											FText::FromString(TEXT("Simple Edit")),
											FText::FromString(TEXT("Open and confirm edits with a single interaction.")),
											&UFortBuildingSettings::bSimpleEdit)
									]
									+ SVerticalBox::Slot().AutoHeight().Padding(0.0f, 0.0f, 0.0f, 6.0f)
									[
										MakeToggleRow(
											FText::FromString(TEXT("Edit On Release")),
											FText::FromString(TEXT("Confirm the selected edit when you release the edit key.")),
											&UFortBuildingSettings::bEditOnRelease)
									]
									+ SVerticalBox::Slot().AutoHeight().Padding(0.0f, 0.0f, 0.0f, 6.0f)
									[
										MakeToggleRow(
											FText::FromString(TEXT("Confirm Edit On Release")),
											FText::FromString(TEXT("Confirm on release when using a separate confirm bind.")),
											&UFortBuildingSettings::bConfirmEditOnRelease)
									]
									+ SVerticalBox::Slot().AutoHeight().Padding(0.0f, 0.0f, 0.0f, 6.0f)
									[
										MakeToggleRow(
											FText::FromString(TEXT("Reset On Release")),
											FText::FromString(TEXT("Reset an unselected edit when the edit key is released.")),
											&UFortBuildingSettings::bResetOnRelease)
									]
									+ SVerticalBox::Slot().AutoHeight().Padding(0.0f, 0.0f, 0.0f, 6.0f)
									[
										MakeToggleRow(
											FText::FromString(TEXT("Disable Pre-Edits")),
											FText::FromString(TEXT("Start each edit with the full, unedited building piece.")),
											&UFortBuildingSettings::bDisablePreEdits)
									]
									+ SVerticalBox::Slot().AutoHeight().Padding(0.0f, 0.0f, 0.0f, 6.0f)
									[
										MakeToggleRow(
											FText::FromString(TEXT("Instant Edit Toggle")),
											FText::FromString(TEXT("Skip the edit-open delay when entering edit mode.")),
											&UFortBuildingSettings::bInstantEditToggle)
									]
									+ SVerticalBox::Slot().AutoHeight().Padding(12.0f, 8.0f, 0.0f, 6.0f)
									[
										MakeSectionHeading(FText::FromString(TEXT("BUILDING")))
									]
									+ SVerticalBox::Slot().AutoHeight().Padding(0.0f, 0.0f, 0.0f, 6.0f)
									[
										MakeToggleRow(
											FText::FromString(TEXT("Turbobuild")),
											FText::FromString(TEXT("Hold the place button to continuously place pieces.")),
											&UFortBuildingSettings::bTurbobuild)
									]
									+ SVerticalBox::Slot().AutoHeight().Padding(0.0f, 0.0f, 0.0f, 6.0f)
									[
										MakeToggleRow(
											FText::FromString(TEXT("Builder Pro Support")),
											FText::FromString(TEXT("Enable separate build-piece binds and hold-to-edit timing.")),
											&UFortBuildingSettings::bBuilderProSupport)
									]
									+ SVerticalBox::Slot().AutoHeight().Padding(12.0f, 8.0f, 0.0f, 6.0f)
									[
										MakeSectionHeading(FText::FromString(TEXT("TIMING")))
									]
									+ SVerticalBox::Slot().AutoHeight().Padding(0.0f, 0.0f, 0.0f, 6.0f)
									[
										MakeNumberRow(
											FText::FromString(TEXT("Edit Hold Time")),
											FText::FromString(TEXT("How long to hold the edit key before the cursor opens (Builder Pro only).")),
											&UFortBuildingSettings::EditHoldTime,
											0.05f,
											0.0f,
											0.5f)
									]
									+ SVerticalBox::Slot().AutoHeight().Padding(0.0f, 0.0f, 0.0f, 6.0f)
									[
										MakeNumberRow(
											FText::FromString(TEXT("Edit Reset Delay")),
											FText::FromString(TEXT("Delay before a release can reset the piece after confirming an edit.")),
											&UFortBuildingSettings::EditResetDelay,
											0.05f,
											0.0f,
											1.0f)
									]
								]
							]
						]
						+ SHorizontalBox::Slot()
						.FillWidth(0.38f)
						[
							SNew(SBorder)
							.Padding(22.0f)
							.BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
							.BorderBackgroundColor(FLinearColor(0.025f, 0.08f, 0.25f, 0.98f))
							[
								SNew(SVerticalBox)
								+ SVerticalBox::Slot()
								.AutoHeight()
								[
									SAssignNew(DetailsTitle, STextBlock)
									.Text(FText::FromString(TEXT("SIMPLE EDIT")))
									.ColorAndOpacity(AccentColor)
									.Font(FCoreStyle::Get().GetFontStyle("LargeFont"))
								]
								+ SVerticalBox::Slot()
								.AutoHeight()
								.Padding(0.0f, 14.0f, 0.0f, 0.0f)
								[
									SNew(SSeparator)
								]
								+ SVerticalBox::Slot()
								.FillHeight(1.0f)
								.Padding(0.0f, 14.0f, 0.0f, 0.0f)
								[
									SAssignNew(DetailsText, STextBlock)
									.Text(FText::FromString(TEXT("Open and confirm edits with a single interaction. This setting is saved automatically and applied to your current building component.")))
									.ColorAndOpacity(TextColor)
									.AutoWrapText(true)
								]
								+ SVerticalBox::Slot()
								.AutoHeight()
								.Padding(0.0f, 12.0f, 0.0f, 0.0f)
								[
									SNew(STextBlock)
									.Text(FText::FromString(TEXT("Changes save as you make them.")))
									.ColorAndOpacity(FLinearColor(0.55f, 0.75f, 0.9f, 1.0f))
								]
							]
						]
					]
					+ SVerticalBox::Slot()
					.AutoHeight()
					.Padding(0.0f, 12.0f, 0.0f, 0.0f)
					[
						SNew(STextBlock)
						.Text(FText::FromString(TEXT("Press F10 to toggle this menu  |  ESC to close")))
						.ColorAndOpacity(FLinearColor(0.55f, 0.75f, 0.9f, 1.0f))
					]
				]
			]
		]
	];
}

TSharedRef<SWidget> SFortBuildingSettingsMenu::MakeSectionHeading(const FText& Heading) const
{
	return SNew(STextBlock)
		.Text(Heading)
		.ColorAndOpacity(AccentColor)
		.Font(FCoreStyle::Get().GetFontStyle("SmallFont"));
}

TSharedRef<SWidget> SFortBuildingSettingsMenu::MakeToggleRow(
	const FText& Label,
	const FText& Description,
	bool UFortBuildingSettings::*Property)
{
	const bool bIsChecked = Settings.IsValid() && (Settings.Get()->*Property);

	return SNew(SBorder)
		.Padding(FMargin(12.0f, 8.0f))
		.BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
		.BorderBackgroundColor(RowColor)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			.VAlign(VAlign_Center)
			[
				SNew(SButton)
				.ButtonStyle(&FCoreStyle::Get().GetWidgetStyle<FButtonStyle>("NoBorder"))
				.HAlign(HAlign_Left)
				.OnClicked_Lambda([this, Label, Description]()
				{
					SelectSetting(Label, Description);
					return FReply::Handled();
				})
				[
					SNew(STextBlock)
					.Text(Label)
					.ColorAndOpacity(TextColor)
					.AutoWrapText(true)
				]
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.Padding(12.0f, 0.0f, 0.0f, 0.0f)
			[
				SNew(SCheckBox)
				.IsChecked(bIsChecked ? ECheckBoxState::Checked : ECheckBoxState::Unchecked)
				.OnCheckStateChanged_Lambda([this, Property](ECheckBoxState NewState)
				{
					if (Settings.IsValid())
					{
						Settings.Get()->*Property = NewState == ECheckBoxState::Checked;
						SaveAndApply();
					}
				})
			]
		];
}

TSharedRef<SWidget> SFortBuildingSettingsMenu::MakeNumberRow(
	const FText& Label,
	const FText& Description,
	float UFortBuildingSettings::*Property,
	float Step,
	float MinValue,
	float MaxValue)
{
	const float InitialValue = Settings.IsValid() ? (Settings.Get()->*Property) : MinValue;
	TSharedRef<STextBlock> ValueText = SNew(STextBlock)
		.Text(FormatSeconds(InitialValue))
		.ColorAndOpacity(TextColor);

	return SNew(SBorder)
		.Padding(FMargin(12.0f, 8.0f))
		.BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
		.BorderBackgroundColor(RowColor)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			.VAlign(VAlign_Center)
			[
				SNew(SButton)
				.ButtonStyle(&FCoreStyle::Get().GetWidgetStyle<FButtonStyle>("NoBorder"))
				.HAlign(HAlign_Left)
				.OnClicked_Lambda([this, Label, Description]()
				{
					SelectSetting(Label, Description);
					return FReply::Handled();
				})
				[
					SNew(STextBlock)
					.Text(Label)
					.ColorAndOpacity(TextColor)
				]
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			[
				SNew(SButton)
				.ButtonColorAndOpacity(FLinearColor(0.06f, 0.30f, 0.62f, 1.0f))
				.OnClicked_Lambda([this, Property, Step, MinValue, MaxValue, ValueText]()
				{
					if (Settings.IsValid())
					{
						float& Value = Settings.Get()->*Property;
						Value = FMath::Clamp(Value - Step, MinValue, MaxValue);
						ValueText->SetText(FormatSeconds(Value));
						SaveAndApply();
					}
					return FReply::Handled();
				})
				[
					SNew(STextBlock).Text(FText::FromString(TEXT("-"))).ColorAndOpacity(TextColor)
				]
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.Padding(8.0f, 0.0f)
			[
				SNew(SBox)
				.WidthOverride(64.0f)
				[
					ValueText
				]
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			[
				SNew(SButton)
				.ButtonColorAndOpacity(FLinearColor(0.06f, 0.30f, 0.62f, 1.0f))
				.OnClicked_Lambda([this, Property, Step, MinValue, MaxValue, ValueText]()
				{
					if (Settings.IsValid())
					{
						float& Value = Settings.Get()->*Property;
						Value = FMath::Clamp(Value + Step, MinValue, MaxValue);
						ValueText->SetText(FormatSeconds(Value));
						SaveAndApply();
					}
					return FReply::Handled();
				})
				[
					SNew(STextBlock).Text(FText::FromString(TEXT("+"))).ColorAndOpacity(TextColor)
				]
			]
		];
}

void SFortBuildingSettingsMenu::SelectSetting(const FText& Label, const FText& Description)
{
	if (DetailsTitle.IsValid())
	{
		DetailsTitle->SetText(Label);
	}
	if (DetailsText.IsValid())
	{
		DetailsText->SetText(Description);
	}
}

void SFortBuildingSettingsMenu::SaveAndApply()
{
	if (!Settings.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Building settings menu lost its settings object."));
		return;
	}

	Settings->SaveSettings();
	if (BuildingComponent.IsValid())
	{
		Settings->ApplyToComponent(BuildingComponent.Get());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Building settings were saved, but no building component is available to apply them to."));
	}
}

void SFortBuildingSettingsMenu::RequestClose()
{
	CloseMenu();
}

FReply SFortBuildingSettingsMenu::CloseMenu()
{
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->RemoveViewportWidgetContent(SharedThis(this));
	}

	if (PlayerController.IsValid())
	{
		PlayerController->bShowMouseCursor = false;
		PlayerController->SetInputMode(FInputModeGameOnly());
	}

	OnClosed.ExecuteIfBound();
	return FReply::Handled();
}

FReply SFortBuildingSettingsMenu::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::Escape || InKeyEvent.GetKey() == EKeys::F10)
	{
		return CloseMenu();
	}

	return SCompoundWidget::OnKeyDown(MyGeometry, InKeyEvent);
}

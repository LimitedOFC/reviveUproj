#pragma once

#include "CoreMinimal.h"
#include "Input/Reply.h"
#include "Brushes/SlateImageBrush.h"
#include "Widgets/SCompoundWidget.h"

class APlayerController;
class UFortBuildingComponent;
class UFortBuildingSettings;
class STextBlock;

class SFortBuildingSettingsMenu : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SFortBuildingSettingsMenu) {}
		SLATE_ARGUMENT(UFortBuildingSettings*, Settings)
		SLATE_ARGUMENT(UFortBuildingComponent*, BuildingComponent)
		SLATE_ARGUMENT(APlayerController*, PlayerController)
		SLATE_EVENT(FSimpleDelegate, OnClosed)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	void RequestClose();

	virtual bool SupportsKeyboardFocus() const override { return true; }
	virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;

private:
	TWeakObjectPtr<UFortBuildingSettings> Settings;
	TWeakObjectPtr<UFortBuildingComponent> BuildingComponent;
	TWeakObjectPtr<APlayerController> PlayerController;
	FSimpleDelegate OnClosed;

	TSharedPtr<FSlateImageBrush> LogoBrush;
	TSharedPtr<STextBlock> DetailsTitle;
	TSharedPtr<STextBlock> DetailsText;

	TSharedRef<SWidget> MakeSectionHeading(const FText& Heading) const;
	TSharedRef<SWidget> MakeToggleRow(
		const FText& Label,
		const FText& Description,
		bool UFortBuildingSettings::*Property);
	TSharedRef<SWidget> MakeNumberRow(
		const FText& Label,
		const FText& Description,
		float UFortBuildingSettings::*Property,
		float Step,
		float MinValue,
		float MaxValue);

	void SelectSetting(const FText& Label, const FText& Description);
	void SaveAndApply();
	FReply CloseMenu();
};

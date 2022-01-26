// Fill out your copyright notice in the Description page of Project Settings.

#include "SLiveLinkAntilatencySourceFactory.h"
#include "ILiveLinkAntilatency.h"
#include "LiveLinkSourceSettings.h"

#include "Widgets/Input/SButton.h"
#include "Widgets/SBoxPanel.h"

#if WITH_EDITOR
#include "DetailLayoutBuilder.h"
#endif


#define LOCTEXT_NAMESPACE "SLiveLinkAntilatencySourceFactory"

void SLiveLinkAntilatencySourceFactory::Construct(const FArguments& args) {
#if WITH_EDITOR
	_onConnectionSettingsAccepted = args._OnConnectionSettingsAccepted;

	FStructureDetailsViewArgs structureDetailsViewArgs;
	FDetailsViewArgs detailsViewArgs;
	detailsViewArgs.bAllowSearch = false;
	detailsViewArgs.bShowScrollBar = false;

	FPropertyEditorModule& propertyEditor = FModuleManager::Get().LoadModuleChecked<FPropertyEditorModule>(TEXT("PropertyEditor"));

	_structOnScope = MakeShared<FStructOnScope>(FLiveLinkAntilatencyConnectionSettings::StaticStruct());
	CastChecked<UScriptStruct>(_structOnScope->GetStruct())->CopyScriptStruct(_structOnScope->GetStructMemory(), &_connectionSettings);
	_structureDetailsView = propertyEditor.CreateStructureDetailView(detailsViewArgs, structureDetailsViewArgs, _structOnScope);

	ChildSlot[
		SNew(SVerticalBox)
		+SVerticalBox::Slot().FillHeight(1.f)[_structureDetailsView->GetWidget().ToSharedRef()]
		+SVerticalBox::Slot().HAlign(HAlign_Right).AutoHeight()[
			SNew(SButton).OnClicked(this, &SLiveLinkAntilatencySourceFactory::OnSettingsAccepted).Text(LOCTEXT("AddSource", "Add"))
		]
	];
#endif
}

FReply SLiveLinkAntilatencySourceFactory::OnSettingsAccepted() {
#if WITH_EDITOR
	CastChecked<UScriptStruct>(_structOnScope->GetStruct())->CopyScriptStruct(&_connectionSettings, _structOnScope->GetStructMemory());
	_onConnectionSettingsAccepted.ExecuteIfBound(_connectionSettings);
#endif

	return FReply::Handled();
}


#undef LOCTEXT_NAMESPACE


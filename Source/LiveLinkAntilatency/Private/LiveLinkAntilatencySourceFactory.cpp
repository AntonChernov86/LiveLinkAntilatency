#include "LiveLinkAntilatencySourceFactory.h"

#include "LiveLinkAntilatencySource.h"
#include "SLiveLinkAntilatencySourceFactory.h"
#include "LiveLinkAntilatencySourceSettings.h"

#include "ILiveLinkClient.h"
#include "Features/IModularFeatures.h"

#define LOCTEXT_NAMESPACE "LiveLinkAntilatencySourceFactory"

FText ULiveLinkAntilatencySourceFactory::GetSourceDisplayName() const {
	return LOCTEXT("SourceDisplayName", "LiveLinkAntilatency Source");
}

FText ULiveLinkAntilatencySourceFactory::GetSourceTooltip() const {
	return LOCTEXT("SourceTooltip", "Allows creation of multiple LiveLink sources using the Antilatency tracking system");
}

TSharedPtr<SWidget> ULiveLinkAntilatencySourceFactory::BuildCreationPanel(FOnLiveLinkSourceCreated onLiveLinkSourceCreated) const {
	return SNew(SLiveLinkAntilatencySourceFactory).
	OnConnectionSettingsAccepted(FOnLiveLinkAntilatencyConnectionSettingAccepted::CreateUObject(
		this,
		&ULiveLinkAntilatencySourceFactory::CreateSourceFromSettings, onLiveLinkSourceCreated
	));
}

TSharedPtr<ILiveLinkSource> ULiveLinkAntilatencySourceFactory::CreateSource(const FString& connectionString) const {
	FLiveLinkAntilatencyConnectionSettings connectionSettings;
	if (!connectionString.IsEmpty()) {
		FLiveLinkAntilatencyConnectionSettings::StaticStruct()->ImportText(
			*connectionString,
			&connectionSettings,
			nullptr,
			PPF_None,
			GLog,
			TEXT("ULiveLinkAntilatencySourceFactory")
		);
	}
	return MakeShared<FLiveLinkAntilatencySource>(connectionSettings);
}

void ULiveLinkAntilatencySourceFactory::CreateSourceFromSettings(FLiveLinkAntilatencyConnectionSettings connectionSettings, FOnLiveLinkSourceCreated onSourceCreated) const {
	FString connectionString;
	FLiveLinkAntilatencyConnectionSettings::StaticStruct()->ExportText(
		connectionString,
		&connectionSettings,
		nullptr,
		nullptr,
		PPF_None,
		nullptr);
	TSharedPtr<FLiveLinkAntilatencySource> sharedPtr = MakeShared<FLiveLinkAntilatencySource>(connectionSettings);
	onSourceCreated.ExecuteIfBound(sharedPtr, MoveTemp(connectionString));
}

#undef LOCTEXT_NAMESPACE


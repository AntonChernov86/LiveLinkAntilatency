//

#pragma once

#include "LiveLinkAntilatencyConnectionSettings.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"

#if WITH_EDITOR
#include "IStructureDetailsView.h"
#endif //WITH_EDITOR

#include "Input/Reply.h"

struct FLiveLinkAntilatencyConnectionSettings;

DECLARE_DELEGATE_OneParam(FOnLiveLinkAntilatencyConnectionSettingAccepted, FLiveLinkAntilatencyConnectionSettings);

class SLiveLinkAntilatencySourceFactory : public SCompoundWidget {
	SLATE_BEGIN_ARGS(SLiveLinkAntilatencySourceFactory)
	{}
		SLATE_EVENT(FOnLiveLinkAntilatencyConnectionSettingAccepted, OnConnectionSettingsAccepted)
	SLATE_END_ARGS()

	void Construct(const FArguments& args);

private:
	FLiveLinkAntilatencyConnectionSettings _connectionSettings;

#if WITH_EDITOR
	TSharedPtr<FStructOnScope> _structOnScope;
	TSharedPtr<IStructureDetailsView> _structureDetailsView;
#endif

	FReply OnSettingsAccepted();
	FOnLiveLinkAntilatencyConnectionSettingAccepted _onConnectionSettingsAccepted;
};
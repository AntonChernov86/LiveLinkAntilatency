// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "LiveLinkSourceFactory.h"
#include "LiveLinkAntilatencySource.h"
#include "LiveLinkAntilatencySourceFactory.generated.h"

/**
 * 
 */
UCLASS()
class LIVELINKANTILATENCY_API ULiveLinkAntilatencySourceFactory : public ULiveLinkSourceFactory {
public:
	GENERATED_BODY()

	virtual FText GetSourceDisplayName() const override;
	virtual FText GetSourceTooltip() const override;

	virtual EMenuType GetMenuType() const override { return EMenuType::SubPanel; }
	virtual TSharedPtr<SWidget> BuildCreationPanel(FOnLiveLinkSourceCreated onLiveLinkSourceCreated) const override;
	virtual TSharedPtr<ILiveLinkSource> CreateSource(const FString& connectionString) const override;

private:
	void CreateSourceFromSettings (FLiveLinkAntilatencyConnectionSettings connectionSettings, FOnLiveLinkSourceCreated onSourceCreated) const;
};

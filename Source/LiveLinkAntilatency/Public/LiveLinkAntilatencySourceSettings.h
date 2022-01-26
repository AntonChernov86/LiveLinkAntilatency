// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LiveLinkSourceSettings.h"
#include "LiveLinkAntilatencySourceSettings.generated.h"

USTRUCT()
struct FAltTrackerSettings {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Alt Tracker Settings")
	float ExtrapolationTime;

	UPROPERTY(EditAnywhere, Category = "Alt Tracker Settings")
	FString EnvironmentCode;

	UPROPERTY(EditAnywhere, Category = "Alt Tracker Settings")
	FString PlacementCode;
};

/**
 * 
 */
UCLASS()
class LIVELINKANTILATENCY_API ULiveLinkAntilatencySourceSettings : public ULiveLinkSourceSettings {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Alt Trackers Settings", meta = (DisplayName = "TrackerSettings (key is a tag of Alt's socket)"))
	TMap<FString ,FAltTrackerSettings> TrackerSettings;
};

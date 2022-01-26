// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"

#include <Antilatency.DeviceNetwork.h>
#include <Antilatency.Alt.Environment.Selector.h>
#include <Antilatency.Alt.Tracking.h>
#include <Antilatency.StorageClient.h>

DECLARE_LOG_CATEGORY_EXTERN(LogLiveLinkAntilatency, Log, All);

class ILiveLinkAntilatencyModule : public IModuleInterface {
public:
	static inline ILiveLinkAntilatencyModule& Get() {
		return FModuleManager::Get().LoadModuleChecked<ILiveLinkAntilatencyModule>(TEXT("LiveLinkAntilatency"));
	}
	
	/** IModuleInterface implementation */
	// virtual void StartupModule() override;
	// virtual void ShutdownModule() override;

	static FString GetPluginName();
	static FString GetPluginDirectory();
	static FString GetPluginBinariesDirectory();

	Antilatency::DeviceNetwork::ILibrary GetDeviceNetworkLibrary();
	Antilatency::Alt::Environment::Selector::ILibrary GetEnvironmentSelectorLibrary();
	Antilatency::Alt::Tracking::ILibrary GetAltTrackingLibrary();
	Antilatency::StorageClient::ILibrary GetStorageClientLibrary();

private:
	Antilatency::DeviceNetwork::ILibrary _adnLibrary;
	Antilatency::Alt::Environment::Selector::ILibrary _environmentSelectorLibrary;
	Antilatency::Alt::Tracking::ILibrary _altTrackingLibrary;
	Antilatency::StorageClient::ILibrary _storageClientLibrary;
};

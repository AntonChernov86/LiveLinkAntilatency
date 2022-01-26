// Copyright Epic Games, Inc. All Rights Reserved.

#include "ILiveLinkAntilatency.h"

#include "Modules/ModuleManager.h"
#include "Misc/Paths.h"

#include <Interfaces/IPluginManager.h>
#include <Antilatency.InterfaceContract.UELibraryLoader.h>

// #define LOCTEXT_NAMESPACE "FLiveLinkAntilatencyModule"

DEFINE_LOG_CATEGORY(LogLiveLinkAntilatency);

class FLiveLinkAntilatencyModule : public ILiveLinkAntilatencyModule {
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

IMPLEMENT_MODULE(FLiveLinkAntilatencyModule, LiveLinkAntilatency)

void FLiveLinkAntilatencyModule::StartupModule() {
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FLiveLinkAntilatencyModule::ShutdownModule() {
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

FString ILiveLinkAntilatencyModule::GetPluginName() {
	return TEXT("LiveLinkAntilatency");
}

FString ILiveLinkAntilatencyModule::GetPluginDirectory() {
	return IPluginManager::Get().FindPlugin(GetPluginName())->GetBaseDir();
}

FString ILiveLinkAntilatencyModule::GetPluginBinariesDirectory() {
	const auto pluginDir = GetPluginDirectory();
	const auto libDir = FPaths::Combine(*pluginDir, TEXT("ThirdParty"), TEXT("Antilatency"), TEXT("AntilatencySdk"), TEXT("Bin"));

	FString result;
#if PLATFORM_WINDOWS
#if PLATFORM_64BITS
	result = FPaths::Combine(*libDir, TEXT("WindowsDesktop"), TEXT("x64"));
#endif
#else
	result = TEXT("");
#endif
	return result;
}

Antilatency::DeviceNetwork::ILibrary ILiveLinkAntilatencyModule::GetDeviceNetworkLibrary() {
	if (_adnLibrary == nullptr) {
		_adnLibrary = LoadAntilatencyLibrary<Antilatency::DeviceNetwork::ILibrary>(FString("AntilatencyDeviceNetwork"), FLiveLinkAntilatencyModule::GetPluginBinariesDirectory());
	}

	if (_adnLibrary == nullptr) {
		UE_LOG(LogLiveLinkAntilatency, Warning, TEXT("Failed to get Antilatecy Device Network ILibrary"));
	}

	return _adnLibrary;
}

Antilatency::Alt::Environment::Selector::ILibrary ILiveLinkAntilatencyModule::GetEnvironmentSelectorLibrary() {
	if (_environmentSelectorLibrary == nullptr) {
		_environmentSelectorLibrary = LoadAntilatencyLibrary<Antilatency::Alt::Environment::Selector::ILibrary>(FString("AntilatencyAltEnvironmentSelector"), FLiveLinkAntilatencyModule::GetPluginBinariesDirectory());
	}

	if (_environmentSelectorLibrary == nullptr) {
		UE_LOG(LogLiveLinkAntilatency, Warning, TEXT("Failed to get Antilatecy Alt Environment Selector ILibrary"));
	}

	return _environmentSelectorLibrary;
}

Antilatency::Alt::Tracking::ILibrary ILiveLinkAntilatencyModule::GetAltTrackingLibrary() {
	if (_altTrackingLibrary == nullptr) {
		_altTrackingLibrary = LoadAntilatencyLibrary<Antilatency::Alt::Tracking::ILibrary>(FString("AntilatencyAltTracking"), FLiveLinkAntilatencyModule::GetPluginBinariesDirectory());
	}

	if (_altTrackingLibrary == nullptr) {
		UE_LOG(LogLiveLinkAntilatency, Warning, TEXT("Failed to get Antilatecy Alt Tracking ILibrary"));
	}

	return _altTrackingLibrary;
}

Antilatency::StorageClient::ILibrary ILiveLinkAntilatencyModule::GetStorageClientLibrary() {
	if (_storageClientLibrary == nullptr) {
		_storageClientLibrary = LoadAntilatencyLibrary<Antilatency::StorageClient::ILibrary>(FString("AntilatencyStorageClient"), FLiveLinkAntilatencyModule::GetPluginBinariesDirectory());
	}

	if (_storageClientLibrary == nullptr) {
		UE_LOG(LogLiveLinkAntilatency, Warning, TEXT("Failed to get Antilatecy Storage Client ILibrary"));
	}

	return _storageClientLibrary;
}

// #undef LOCTEXT_NAMESPACE
	

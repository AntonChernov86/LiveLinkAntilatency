#pragma once

#include "ILiveLinkSource.h"
#include "LiveLinkAntilatencyConnectionSettings.h"
#include "LiveLinkAntilatencySourceSettings.h"
#include "AltTracker.h"

#include <Antilatency.DeviceNetwork.h>
#include <Antilatency.Alt.Tracking.h>
#include <Antilatency.Alt.Environment.h>
#include <Antilatency.Alt.Environment.Selector.h>
#include <Antilatency.StorageClient.h>

struct ULiveLinkAntilatencySettings;
class ILiveLinkClient;

class LIVELINKANTILATENCY_API FLiveLinkAntilatencySource : public ILiveLinkSource, public FRunnable, public TSharedFromThis<FLiveLinkAntilatencySource>{
public:
	FLiveLinkAntilatencySource(const FLiveLinkAntilatencyConnectionSettings& connectionSettings);

	virtual ~FLiveLinkAntilatencySource() override;

	// Begin ILiveLinkSource Interface
	virtual void ReceiveClient(ILiveLinkClient* inClient, FGuid inSourceGuid) override;
	virtual void InitializeSettings(ULiveLinkSourceSettings* settings) override;

	virtual bool IsSourceStillValid() const override;

	virtual bool RequestSourceShutdown() override;

	virtual FText GetSourceType() const override { return _sourceType; };
	virtual FText GetSourceMachineName() const override { return _sourceMachineName; }
	virtual FText GetSourceStatus() const override { return _sourceStatus; }

	virtual TSubclassOf<ULiveLinkSourceSettings> GetSettingsClass() const override { return ULiveLinkAntilatencySourceSettings::StaticClass(); }
	virtual void OnSettingsChanged(ULiveLinkSourceSettings* settings, const FPropertyChangedEvent& propertyChangedEvent) override;
	// End ILiveLinkSource Interface

	// Begin FRunnable Interface
	virtual bool Init() override;
	virtual uint32 Run() override;
	void Start();
	virtual void Stop() override;
	virtual void Exit() override { }
	// End FRunnable Interface

	void Send(FLiveLinkFrameDataStruct* frameDataToSend, FName subjectName);

private:
	bool CreateEnvironment(FString code, Antilatency::Alt::Environment::IEnvironment& result);
	bool CreatePlacement(FString code, Antilatency::Math::floatP3Q& result);
	
	ILiveLinkClient* _client;

	// Our identifier in LiveLink
	FGuid _sourceGuid;

	FText _sourceType;
	FText _sourceMachineName;
	FText _sourceStatus;

	// Threadsafe Bool for terminating the main thread loop
	FThreadSafeBool _stopping;

	// Thread to run socket operations on
	FRunnableThread* _thread;

	// Name of the sockets thread
	FString _threadName;

	// Deferred start delegate handle.
	FDelegateHandle _deferredStartDelegateHandle;

	// Pointer to the settings for this source so we don't have to duplicate data
	ULiveLinkAntilatencySourceSettings* _savedSourceSettings;

	// List of subjects we've already encountered
	TSet<FName> _encounteredSubjects;

	TArray<FUsbDeviceFilter> _usbDeviceFilters;
	TArray<FIpDeviceFilter> _ipDeviceFilters;

	FThreadSafeBool _forceSearchForTrackers = false;

	Antilatency::DeviceNetwork::ILibrary _adnLibrary;
	Antilatency::Alt::Tracking::ILibrary _altTrackingLibrary;
	Antilatency::Alt::Environment::Selector::ILibrary _environmentSelectorLibrary;
	Antilatency::StorageClient::ILibrary _storageClientLibrary;

	Antilatency::DeviceNetwork::INetwork _network;
	uint32_t _updateId = 0;

	Antilatency::Alt::Environment::IEnvironment _defaultEnvironment;
	Antilatency::Math::floatP3Q _defaultPlacement;
	bool _defaultPlacementInitialized = false;

	Antilatency::Alt::Tracking::ITrackingCotaskConstructor _trackingCotaskConstructor;

	TArray<AltTracker> _runningTasks;
};

#include "LiveLinkAntilatencySource.h"

#include "ExceptionCheck.h"
#include "ILiveLinkClient.h"
#include "ILiveLinkAntilatency.h"
#include "Roles/LiveLinkTransformRole.h"
#include "Roles/LiveLinkTransformTypes.h"

#define LOCTEXT_NAMESPACE "LiveLinkAntilatencySource"

FLiveLinkAntilatencySource::FLiveLinkAntilatencySource(const FLiveLinkAntilatencyConnectionSettings& connectionSettings) : _client(nullptr), _stopping(false), _thread(nullptr) {
	_sourceStatus = LOCTEXT("SourceStatus_NoData", "No data");
	_sourceType = LOCTEXT("SourceType_Antilatency", "Antilatency");
	_sourceMachineName = LOCTEXT("AntilatencyMachineName", "Local");

	_usbDeviceFilters = connectionSettings.UsbDeviceFilters;
	_ipDeviceFilters = connectionSettings.IpDeviceFilters;

	if (_usbDeviceFilters.Num() == 0 && _ipDeviceFilters.Num() == 0) {
		UE_LOG(LogLiveLinkAntilatency, Error, TEXT("LiveLinkAntilatencySource: USB and IP device filters are both empty"));
		return;
	}

	_deferredStartDelegateHandle = FCoreDelegates::OnEndFrame.AddRaw(this, &FLiveLinkAntilatencySource::Start);
}

FLiveLinkAntilatencySource::~FLiveLinkAntilatencySource() {
	if (_deferredStartDelegateHandle.IsValid()) {
		FCoreDelegates::OnEndFrame.Remove(_deferredStartDelegateHandle);
	}

	Stop();

	if (_thread != nullptr) {
		_thread->WaitForCompletion();
		delete _thread;
		_thread = nullptr;
	}
}

void FLiveLinkAntilatencySource::ReceiveClient(ILiveLinkClient* inClient, FGuid inSourceGuid) {
	_client = inClient;
	_sourceGuid = inSourceGuid;
}

void FLiveLinkAntilatencySource::InitializeSettings(ULiveLinkSourceSettings* settings) {
	_savedSourceSettings = Cast<ULiveLinkAntilatencySourceSettings>(settings);
}

bool FLiveLinkAntilatencySource::IsSourceStillValid() const {
	return !_stopping && (_thread != nullptr);
}

bool FLiveLinkAntilatencySource::RequestSourceShutdown() {
	Stop();
	return true;
}

void FLiveLinkAntilatencySource::OnSettingsChanged(ULiveLinkSourceSettings* settings, const FPropertyChangedEvent& propertyChangedEvent) {
	ILiveLinkSource::OnSettingsChanged(settings, propertyChangedEvent);

	FProperty* memberProperty = propertyChangedEvent.MemberProperty;
	FProperty* property = propertyChangedEvent.Property;
	if (property && memberProperty && (propertyChangedEvent.ChangeType != EPropertyChangeType::Interactive)) {
		ULiveLinkAntilatencySourceSettings* sourceSettings = Cast<ULiveLinkAntilatencySourceSettings>(settings);
		if (_savedSourceSettings != sourceSettings) {
			UE_LOG(LogLiveLinkAntilatency, Error, TEXT("LiveLinkAntilatencySource: OnSettingsChanged pointers don't match - this should never happen!"));
			return;
		}

		_forceSearchForTrackers = true;

		// TODO: remove active tracker if it settings has been changed
	}
}

bool FLiveLinkAntilatencySource::Init() {
	auto module = ILiveLinkAntilatencyModule::Get();
	_adnLibrary = module.GetDeviceNetworkLibrary();
	_altTrackingLibrary = module.GetAltTrackingLibrary();
	_environmentSelectorLibrary = module.GetEnvironmentSelectorLibrary();
	_storageClientLibrary = module.GetStorageClientLibrary();

	if (_adnLibrary == nullptr || _altTrackingLibrary == nullptr || _environmentSelectorLibrary == nullptr || _storageClientLibrary == nullptr) {
		return false;
	}
	
	auto exception = _altTrackingLibrary.createTrackingCotaskConstructor(_trackingCotaskConstructor);
	ANTILATENCY_EXCEPTION_CHECK(exception, _altTrackingLibrary);
	if (exception != Antilatency::InterfaceContract::ExceptionCode::Ok || _trackingCotaskConstructor == nullptr) {
		return false;
	}

	Antilatency::DeviceNetwork::IDeviceFilter deviceFilter;
	exception = _adnLibrary.createFilter(deviceFilter);
	ANTILATENCY_EXCEPTION_CHECK(exception, _adnLibrary);
	if (exception != Antilatency::InterfaceContract::ExceptionCode::Ok || deviceFilter == nullptr) {
		return false;
	}

	if (_usbDeviceFilters.Num() > 0) {
		for (auto f : _usbDeviceFilters) {
			exception = deviceFilter.addUsbDevice(f.ToNative());
			ANTILATENCY_EXCEPTION_CHECK(exception, deviceFilter);
			if (exception != Antilatency::InterfaceContract::ExceptionCode::Ok) {
				return false;
			}
		}
	}

	if (_ipDeviceFilters.Num() > 0) {
		for (auto f: _ipDeviceFilters) {
			auto ip = std::string(TCHAR_TO_UTF8(*(f.Ip)));
			auto mask = std::string(TCHAR_TO_UTF8(*(f.Mask)));
			exception = deviceFilter.addIpDevice(ip, mask);
			ANTILATENCY_EXCEPTION_CHECK(exception, deviceFilter);
			if (exception != Antilatency::InterfaceContract::ExceptionCode::Ok) {
				return false;
			}
		}
	}

	exception = _adnLibrary.createNetwork(deviceFilter, _network);
	ANTILATENCY_EXCEPTION_CHECK(exception, _adnLibrary);
	if (exception != Antilatency::InterfaceContract::ExceptionCode::Ok || _network == nullptr) {
		return false;
	}
	
	return true;
}

uint32 FLiveLinkAntilatencySource::Run() {
	
	while (!_stopping) {
		uint32_t curUpdateId;
		auto exception = _network.getUpdateId(curUpdateId);
		ANTILATENCY_EXCEPTION_CHECK(exception, _network);
		if (exception != Antilatency::InterfaceContract::ExceptionCode::Ok) {
			break;
		}

		if (_updateId != curUpdateId || _forceSearchForTrackers) {
			_updateId = curUpdateId;

			UE_LOG(LogLiveLinkAntilatency, Log, TEXT("Device network changed, update ID: %d"), _updateId);

			_runningTasks.RemoveAll([](const AltTracker tracker) {
				return tracker.IsFinished();
			});

			std::vector<Antilatency::DeviceNetwork::NodeHandle> trackingNodes;
			exception = _trackingCotaskConstructor.findSupportedNodes(_network, trackingNodes);
			ANTILATENCY_EXCEPTION_CHECK(exception, _trackingCotaskConstructor);
			if (exception != Antilatency::InterfaceContract::ExceptionCode::Ok) {
				break;
			}

			for (auto node : trackingNodes) {
				Antilatency::DeviceNetwork::NodeStatus status;
				exception = _network.nodeGetStatus(node, status);
				ANTILATENCY_EXCEPTION_CHECK(exception, _network);
				if (exception != Antilatency::InterfaceContract::ExceptionCode::Ok) {
					continue;
				}

				if (status != Antilatency::DeviceNetwork::NodeStatus::Idle) {
					continue;
				}

				Antilatency::DeviceNetwork::NodeHandle parent;
				exception = _network.nodeGetParent(node, parent);
				ANTILATENCY_EXCEPTION_CHECK(exception, _trackingCotaskConstructor);
				if (exception != Antilatency::InterfaceContract::ExceptionCode::Ok) {
					continue;
				}

				if (parent == Antilatency::DeviceNetwork::NodeHandle::Null) {
					UE_LOG(LogLiveLinkAntilatency, Error, TEXT("Parent node == root"));
					continue;
				}

				std::string tagStr;
				exception = _network.nodeGetStringProperty(parent, "Tag", tagStr);
				ANTILATENCY_EXCEPTION_CHECK(exception, _trackingCotaskConstructor);
				if (exception != Antilatency::InterfaceContract::ExceptionCode::Ok) {
					continue;
				}

				auto tag = FString(tagStr.c_str());
				if (_savedSourceSettings == nullptr) {
					UE_LOG(LogLiveLinkAntilatency, Warning, TEXT("Failed to get settings"));
					continue;
				}
				auto settings = _savedSourceSettings->TrackerSettings.Find(tag);

				if (settings == nullptr) {
					UE_LOG(LogLiveLinkAntilatency, Warning, TEXT("Failed to find settings for Alt in socket with tag %s"), *tag);
					continue;
				}

				Antilatency::Alt::Environment::IEnvironment environment; 
				if (!CreateEnvironment(settings->EnvironmentCode, environment)) {
					UE_LOG(LogLiveLinkAntilatency, Warning, TEXT("Failed to create environment"));
					continue;
				}

				Antilatency::Math::floatP3Q placement;
				if (!CreatePlacement(settings->PlacementCode, placement)) {
					UE_LOG(LogLiveLinkAntilatency, Warning, TEXT("Failed to create placement"));
					continue;
				}
				
				Antilatency::Alt::Tracking::ITrackingCotask cotask;
				exception = _trackingCotaskConstructor.startTask(_network, node, environment, cotask);
				ANTILATENCY_EXCEPTION_CHECK(exception, _trackingCotaskConstructor);
                if (exception != Antilatency::InterfaceContract::ExceptionCode::Ok) {
                	continue;
                }
				
				auto tracker = AltTracker(cotask, tag, settings->ExtrapolationTime, placement);
				_runningTasks.Add(tracker);
			}

			_forceSearchForTrackers = false;
		}

		for (auto tracker : _runningTasks) {
			FVector position;
			FQuat rotation;
			
			if (!tracker.GetExtrapolatedState(position, rotation)) {
				continue;
			}

			auto name = tracker.GetTrackerName();

			FLiveLinkFrameDataStruct frameData(FLiveLinkTransformFrameData::StaticStruct());
			FLiveLinkTransformFrameData* transformFrameData = frameData.Cast<FLiveLinkTransformFrameData>();
			transformFrameData->Transform = FTransform(rotation, position);

			Send(&frameData, FName(name));
		}

		FPlatformProcess::Sleep(0.016667f);
	}

	_runningTasks.Empty();

	return 0;
}

void FLiveLinkAntilatencySource::Start() {
	check(_deferredStartDelegateHandle.IsValid());

	FCoreDelegates::OnEndFrame.Remove(_deferredStartDelegateHandle);
	_deferredStartDelegateHandle.Reset();

	_sourceStatus = LOCTEXT("SourceStatus_Receving", "Receving");
	_threadName = "LiveLinkAntilatency Receiver ";
	_threadName.AppendInt(FAsyncThreadIndex::GetNext());

	_thread = FRunnableThread::Create(this, *_threadName, 128 * 1024, TPri_Normal, FPlatformAffinity::GetPoolThreadMask());
}

void FLiveLinkAntilatencySource::Stop() {
	_stopping = true;
}

void FLiveLinkAntilatencySource::Send(FLiveLinkFrameDataStruct* frameDataToSend, FName subjectName) {
	if (_stopping || _client == nullptr) {
		return;
	}

	if (!_encounteredSubjects.Contains(subjectName)) {
		FLiveLinkStaticDataStruct staticData(FLiveLinkTransformStaticData::StaticStruct());
		_client->PushSubjectStaticData_AnyThread({_sourceGuid, subjectName}, ULiveLinkTransformRole::StaticClass(), MoveTemp(staticData));
		_encounteredSubjects.Add(subjectName);
	}

	_client->PushSubjectFrameData_AnyThread({_sourceGuid, subjectName}, MoveTemp(*frameDataToSend));
}

bool FLiveLinkAntilatencySource::CreateEnvironment(FString code, Antilatency::Alt::Environment::IEnvironment& result) {
	if (_environmentSelectorLibrary == nullptr) {
		return false;
	}

	Antilatency::InterfaceContract::ExceptionCode exception;

	if (code.IsEmpty()) {
		if (_storageClientLibrary == nullptr) {
			return false;
		}

		Antilatency::StorageClient::IStorage storage;
		exception = _storageClientLibrary.getLocalStorage(storage);
		ANTILATENCY_EXCEPTION_CHECK(exception, _storageClientLibrary);
		if (exception != Antilatency::InterfaceContract::ExceptionCode::Ok || storage == nullptr) {
			return false;
		}

		std::string environmentCodeStr;
		exception = storage.read("environment", "default", environmentCodeStr);
		ANTILATENCY_EXCEPTION_CHECK(exception, _storageClientLibrary);
		if (exception != Antilatency::InterfaceContract::ExceptionCode::Ok) {
			return false;
		}

		code = {environmentCodeStr.c_str()};
	}

	exception = _environmentSelectorLibrary.createEnvironment(std::string(TCHAR_TO_UTF8(*code)), result);
    ANTILATENCY_EXCEPTION_CHECK(exception, _environmentSelectorLibrary);
    if (exception != Antilatency::InterfaceContract::ExceptionCode::Ok || result == nullptr) {
    	return false;
    }

	return true;
}

bool FLiveLinkAntilatencySource::CreatePlacement(FString code, Antilatency::Math::floatP3Q& result) {
	if (_altTrackingLibrary == nullptr) {
		return false;
	}

	Antilatency::InterfaceContract::ExceptionCode exception;

	if (code.IsEmpty()) {
		if (_storageClientLibrary == nullptr) {
			return false;
		}

		Antilatency::StorageClient::IStorage storage;
		exception = _storageClientLibrary.getLocalStorage(storage);
		ANTILATENCY_EXCEPTION_CHECK(exception, _storageClientLibrary);
		if (exception != Antilatency::InterfaceContract::ExceptionCode::Ok || storage == nullptr) {
			return false;
		}

		std::string placementCodeStr;
		exception = storage.read("placement", "default", placementCodeStr);
		ANTILATENCY_EXCEPTION_CHECK(exception, _storageClientLibrary);
		if (exception != Antilatency::InterfaceContract::ExceptionCode::Ok) {
			return false;
		}

		code = {placementCodeStr.c_str()};
	}

	exception = _altTrackingLibrary.createPlacement(std::string(TCHAR_TO_UTF8(*code)), result);
	ANTILATENCY_EXCEPTION_CHECK(exception, _altTrackingLibrary);
	if (exception != Antilatency::InterfaceContract::ExceptionCode::Ok) {
		return false;
	}

	return true;
}


#undef LOCTEXT_NAMESPACE

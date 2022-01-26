// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include <Antilatency.DeviceNetwork.h>

#include "LiveLinkAntilatencyConnectionSettings.generated.h"

/**
 * USB device identifier.
 */
USTRUCT()
struct FUsbDeviceFilter {
	GENERATED_BODY()

	FUsbDeviceFilter() {
		Vid = static_cast<int32>(Antilatency::DeviceNetwork::Constants::AllUsbDevices.vid);
		Pid = static_cast<int32>(Antilatency::DeviceNetwork::Constants::AllUsbDevices.pid);
		PidMask = static_cast<int32>(Antilatency::DeviceNetwork::Constants::AllUsbDevices.pidMask);
	}
    
	FUsbDeviceFilter(Antilatency::DeviceNetwork::UsbDeviceFilter native) :
		Vid(static_cast<int32>(native.vid)),
		Pid(static_cast<int32>(native.pid)),
		PidMask(static_cast<int32>(native.pidMask)){}

	FORCEINLINE bool operator==(const FUsbDeviceFilter& other) const {
		return Vid == other.Vid && Pid == other.Pid && PidMask == other.PidMask;
	}

	FORCEINLINE bool operator!=(const FUsbDeviceFilter& other) const {
		return Vid != other.Vid || Pid != other.Pid || PidMask != other.PidMask;
	}

	Antilatency::DeviceNetwork::UsbDeviceFilter ToNative() const {
		Antilatency::DeviceNetwork::UsbDeviceFilter result;
		result.vid = static_cast<Antilatency::DeviceNetwork::UsbVendorId>(Vid & 0xFFFF);
		result.pid = static_cast<uint16_t>(Pid & 0xFFFF);
		result.pidMask = static_cast<uint16_t>(PidMask & 0xFFFF);
		return result;
	}

	/** USB device vendor ID. */
	UPROPERTY(EditAnywhere, Category = "USB Device Filter")
	int32 Vid;

	/** USB device product ID. */
	UPROPERTY(EditAnywhere, Category = "USB Device Filter")
	int32 Pid;

	UPROPERTY(EditAnywhere, Category = "USB Device Filter")
	int32 PidMask;
};

USTRUCT()
struct FIpDeviceFilter {
	GENERATED_BODY()

	FIpDeviceFilter() {
		Ip = {Antilatency::DeviceNetwork::Constants::AllIpDevicesIp};
		Mask = {Antilatency::DeviceNetwork::Constants::AllIpDevicesMask};
	}

	UPROPERTY(EditAnywhere, Category = "IP Device Filter")
	FString Ip;

	UPROPERTY(EditAnywhere, Category = "IP Device Filter")
	FString Mask;
	
};

/**
 * 
 */
USTRUCT()
struct LIVELINKANTILATENCY_API FLiveLinkAntilatencyConnectionSettings {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Connection Settings", meta = (ClampMin = 1, ClampMax = 1000))
	uint32 LocalUpdateRateInHz = 100;
	
	UPROPERTY(EditAnywhere, Category = "Connection Settings")
	TArray<FUsbDeviceFilter> UsbDeviceFilters = { FUsbDeviceFilter{} };

	UPROPERTY(EditAnywhere, Category = "Connection Settings")
	TArray<FIpDeviceFilter> IpDeviceFilters;
};

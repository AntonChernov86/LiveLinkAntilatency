// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Antilatency.DeviceNetwork.h>
#include <Antilatency.Alt.Tracking.h>


class LIVELINKANTILATENCY_API AltTracker {

public:
	AltTracker(Antilatency::Alt::Tracking::ITrackingCotask cotask, const FString name, const float extrapolationTime, Antilatency::Math::floatP3Q placement)
		: ExtrapolationTime(extrapolationTime),
		Placement(placement),
		_name(name),
		_cotask(cotask){}

	float ExtrapolationTime = 0.0f;
	
	Antilatency::Math::floatP3Q Placement;

	FString GetTrackerName() const {
		return _name;
	}

	bool IsFinished() const;
	
	bool GetExtrapolatedState(FVector& position, FQuat& rotation) const;

private:
	FString _name;
	mutable Antilatency::Alt::Tracking::ITrackingCotask _cotask;



};

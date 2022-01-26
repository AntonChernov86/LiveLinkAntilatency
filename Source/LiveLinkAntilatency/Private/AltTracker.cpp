#include "AltTracker.h"

#include <ExceptionCheck.h>

bool AltTracker::IsFinished() const {
	if (_cotask == nullptr) {
		return true;
	}

	Antilatency::InterfaceContract::LongBool result;
	auto exception = _cotask.isTaskFinished(result);
	ANTILATENCY_EXCEPTION_CHECK(exception, _cotask);

	if (exception != Antilatency::InterfaceContract::ExceptionCode::Ok) {
		return true;
	}

	return static_cast<bool>(result);
}

bool AltTracker::GetExtrapolatedState(FVector& position, FQuat& rotation) const {
	if (_cotask == nullptr) {
		return false;
	}

	Antilatency::Alt::Tracking::State state;
	auto exception = _cotask.getExtrapolatedState(Placement, ExtrapolationTime, state);
	ANTILATENCY_EXCEPTION_CHECK(exception, _cotask);
	if (exception != Antilatency::InterfaceContract::ExceptionCode::Ok) {
		return false;
	}

	if (state.stability.stage == Antilatency::Alt::Tracking::Stage::InertialDataInitialization) {
		return false;
	}
	
	position = FVector(state.pose.position.z * 100.0f, state.pose.position.x * 100.0f, state.pose.position.y * 100.0f);
	rotation = FQuat(state.pose.rotation.z, state.pose.rotation.x, state.pose.rotation.y, state.pose.rotation.w);
	
	return true;
}

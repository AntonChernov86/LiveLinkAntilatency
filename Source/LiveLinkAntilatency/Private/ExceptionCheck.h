#pragma once

#include "CoreMinimal.h"
#include "ILiveLinkAntilatency.h"

#include <type_traits>

#include <Antilatency.InterfaceContract.h>

#define ANTILATENCY_EXCEPTION_CHECK(exception, obj) \
{ \
	static_assert(std::is_base_of<Antilatency::InterfaceContract::IUnsafe, decltype(obj)>::value, "Type mismatch for interace object"); \
	static_assert(std::is_same<decltype(exception), Antilatency::InterfaceContract::ExceptionCode>::value, "Type mismatch for exception"); \
	if(exception != Antilatency::InterfaceContract::ExceptionCode::Ok) { \
		auto exceptionInterface = obj.queryInterface<Antilatency::InterfaceContract::IExceptionData>(); \
		if(exceptionInterface == nullptr) { \
			UE_LOG(LogLiveLinkAntilatency, Error, TEXT("Failed to read exception message")); \
		}else{ \
			auto msg = exceptionInterface.getMessage(); \
			UE_LOG(LogLiveLinkAntilatency, Warning, TEXT("%s"), UTF8_TO_TCHAR(msg.c_str())); \
		} \
	} \
}

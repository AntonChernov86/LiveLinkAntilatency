// Copyright 2021, ALT LLC. All Rights Reserved.
// This file is part of Antilatency SDK.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution and at http://www.antilatency.com/eula
// You may not use this file except in compliance with the License.
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Copyright 2016-2019, Antilatency, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#if PLATFORM_ANDROID

#include <dlfcn.h>
#include "Android/AndroidApplication.h"
#include "Android/AndroidJNI.h"

#endif

#include <Antilatency.Api.h>
#include <Antilatency.InterfaceContract.h>

namespace Antilatency {
    namespace InterfaceContract {
        template<typename LibraryInterface>
        class UELibraryLoader : public Antilatency::InterfaceContract::Library<LibraryInterface> {
        public:
            UELibraryLoader(FString libraryName, FString pluginBinariesDirectory) {
#if PLATFORM_WINDOWS
                libraryName += ".dll";
                _library = FPlatformProcess::GetDllHandle(*libraryName);
                if (_library == nullptr) {
                    auto libraryPath = FPaths::Combine(pluginBinariesDirectory, libraryName);
                    _library = FPlatformProcess::GetDllHandle(*libraryPath);
                }
#elif PLATFORM_ANDROID
                libraryName = "lib" + libraryName + ".so";
                auto error = dlerror();
                _library = dlopen(TCHAR_TO_ANSI(*libraryName), RTLD_LAZY);
                error = dlerror();
                if (error != NULL) {
                    UE_LOG(LogTemp, Error, TEXT("Failed to load %s: %s"), *libraryName, UTF8_TO_TCHAR(error));
                }
#endif
            }
            typename Library<LibraryInterface>::LibraryEntryPoint getEntryPoint() override {
                if (_library != nullptr) {
#if PLATFORM_WINDOWS
                    return reinterpret_cast<typename Library<LibraryInterface>::LibraryEntryPoint>(FPlatformProcess::GetDllExport(_library, ANSI_TO_TCHAR("getLibraryInterface")));
#elif PLATFORM_ANDROID
                    return reinterpret_cast<typename Library<LibraryInterface>::LibraryEntryPoint>(dlsym(_library, "getLibraryInterface"));
#endif
                }
                return nullptr;
            }
            void unloadLibraryImpl() override {}
        private:
            void* _library = nullptr;
        };

        template<typename LibraryInterface>
        LibraryInterface getLibraryInterface(FString libraryName, FString pluginBinariesDirectory) {
            return (new UELibraryLoader<LibraryInterface>(libraryName, pluginBinariesDirectory))->getLibraryInterface();
        }
    }
}

template<typename T> 
T LoadAntilatencyLibrary(FString libraryName, FString pluginBinariesDirectory) {
    auto libraryInterface = Antilatency::InterfaceContract::getLibraryInterface<T>(libraryName, pluginBinariesDirectory);

#if PLATFORM_ANDROID  
    if (libraryInterface != nullptr) {
        JNIEnv* env = FAndroidApplication::GetJavaEnv(true);
        JavaVM* vm = nullptr;
        env->GetJavaVM(&vm);
        FAndroidApplication::CheckJavaException();
        jobject activity = FAndroidApplication::GetGameActivityThis();
        FAndroidApplication::CheckJavaException();

        auto libraryJNI = libraryInterface.template queryInterface<AndroidJniWrapper::IAndroidJni>();
        if (libraryJNI != nullptr) {
            libraryJNI.initJni(vm, activity);
        }
    }
#endif
    return libraryInterface;
}
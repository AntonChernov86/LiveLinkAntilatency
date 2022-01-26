using System;
using System.IO;

using UnrealBuildTool;

public class LiveLinkAntilatency : ModuleRules {
	public LiveLinkAntilatency(ReadOnlyTargetRules Target) : base(Target) {
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		var baseDirectory = Path.GetFullPath(Path.Combine(ModuleDirectory, "..", ".."));
		var binaryDirectory = Path.Combine(baseDirectory, "ThirdParty", "Antilatency", "AntilatencySdk", "Bin", Target.Platform.ToString());
		var headersDirectory = Path.Combine(baseDirectory, "ThirdParty", "Antilatency", "AntilatencySdk", "Api");
		var toolsDirectory = Path.Combine(baseDirectory, "ThirdParty", "Antilatency", "Tools");
		
#if UE_4_27_OR_LATER
		bUsePrecompiled = false;
#endif
		PublicIncludePaths.AddRange(new string[] {
			headersDirectory,
			toolsDirectory
		});
		
		PublicDependencyModuleNames.AddRange(
			new string[] {
				"Core",
				"LiveLinkInterface",
				"Projects"
			}
		);
		
		PrivateDependencyModuleNames.AddRange(
			new string[] {
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
			}
		);
		
        if (Target.Platform == UnrealTargetPlatform.Win64) {
	        var deviceNetworkLibraryPath = Path.Combine(binaryDirectory, "AntilatencyDeviceNetwork.dll");
			var altTrackingLibraryPath = Path.Combine(binaryDirectory, "AntilatencyAltTracking.dll");
			var environmentSelectorLibraryPath = Path.Combine(binaryDirectory, "AntilatencyAltEnvironmentSelector.dll");
			var environmentHorizontalGridLibraryPath = Path.Combine(binaryDirectory, "AntilatencyAltEnvironmentHorizontalGrid.dll");
			var environmentPillarsLibraryPath = Path.Combine(binaryDirectory, "AntilatencyAltEnvironmentPillars.dll");
			var environmentArbitrary2DLibraryPath = Path.Combine(binaryDirectory, "AntilatencyAltEnvironmentArbitrary2D.dll");
			var storageClientLibraryPath = Path.Combine(binaryDirectory, "AntilatencyStorageClient.dll");

#if UE_4_19_OR_LATER
			RuntimeDependencies.Add(deviceNetworkLibraryPath);
			RuntimeDependencies.Add(altTrackingLibraryPath);
			RuntimeDependencies.Add(environmentSelectorLibraryPath);
			RuntimeDependencies.Add(environmentHorizontalGridLibraryPath);
			RuntimeDependencies.Add(environmentPillarsLibraryPath);
			RuntimeDependencies.Add(environmentArbitrary2DLibraryPath);
			RuntimeDependencies.Add(storageClientLibraryPath);
#else
            RuntimeDependencies.Add(new RuntimeDependency(deviceNetworkLibraryPath));
			RuntimeDependencies.Add(new RuntimeDependency(altTrackingLibraryPath));
			RuntimeDependencies.Add(new RuntimeDependency(environmentSelectorLibraryPath));
			RuntimeDependencies.Add(new RuntimeDependency(environmentHorizontalGridLibraryPath));
			RuntimeDependencies.Add(new RuntimeDependency(environmentPillarsLibraryPath));
			RuntimeDependencies.Add(new RuntimeDependency(environmentArbitrary2DLibraryPath));
			RuntimeDependencies.Add(new RuntimeDependency(storageClientLibraryPath));
#endif
		} else {
			throw new NotImplementedException("NOT IMPLEMENTED YET FOR PLATFORM " + Target.Platform.ToString());
		}
	}
}

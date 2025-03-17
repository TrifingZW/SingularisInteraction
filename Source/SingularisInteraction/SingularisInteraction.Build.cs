// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SingularisInteraction : ModuleRules
{
	public SingularisInteraction(ReadOnlyTargetRules target) : base(target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			[
				// ... 在此处添加公众包含路径 ...
			]
		);


		PrivateIncludePaths.AddRange(
			[
				// ... 在此处添加其他私人包含路径 ...
			]
		);


		PublicDependencyModuleNames.AddRange(
			[
				"Core",
				"UMG",
				"Slate",
				"SlateCore",
				"InputCore",
				"EnhancedInput"
				// ... add other public dependencies that you statically link with here ...
			]
		);


		PrivateDependencyModuleNames.AddRange(
			[
				"CoreUObject",
				"Engine",
				// ... add private dependencies that you statically link with here ...	
			]
		);


		DynamicallyLoadedModuleNames.AddRange(
			[
				// ... add any modules that your module loads dynamically here ...
			]
		);
	}
}
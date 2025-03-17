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
				// ... 添加您在此处静态链接的其他公共依赖性 ...
			]
		);


		PrivateDependencyModuleNames.AddRange(
			[
				"CoreUObject",
				"Engine",
				// ... 添加您在此处静态链接的私人依赖项 ...	
			]
		);


		DynamicallyLoadedModuleNames.AddRange(
			[
				// ... 在此处添加模块动态加载的所有模块 ...
			]
		);
	}
}
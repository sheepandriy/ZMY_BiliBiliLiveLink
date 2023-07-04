// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ZMY_BiliBiliLiveLink : ModuleRules
{
	public ZMY_BiliBiliLiveLink(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"HTTP",
                "WebSockets",
                "Json",
                "JsonUtilities"
				// ... add private dependencies that you statically link with here ...	
			}
			);

        AddEngineThirdPartyPrivateStaticDependencies(Target, "OpenSSL", "zlib");
    }
}

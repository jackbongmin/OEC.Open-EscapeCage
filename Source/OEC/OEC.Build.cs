// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class OEC : ModuleRules
{
	public OEC(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore",
			
			// 입력 시스템
			"EnhancedInput",

			// GAS 필수 모듈
			"GameplayAbilities",
			"GameplayTags",
			"GameplayTasks",

			// UI 시스템
			"UMG",

			// AI 시스템
			"AIModule",
			"NavigationSystem",

            "Niagara",
        });

		PrivateDependencyModuleNames.AddRange(new string[] {
			// UMG를 쓰기위한 필수 UI코어 모듈
			"Slate",
			"SlateCore",
        });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}

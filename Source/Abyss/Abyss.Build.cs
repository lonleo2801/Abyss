// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Abyss : ModuleRules
{
	public Abyss(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"InputDevice",
			// 应用程序级功能（如窗口管理、输入事件）。
			"ApplicationCore",
			//UI
			"UMG",
			"Slate",
			"ModelViewViewModel",
			//GAS
			"GameplayAbilities",
			"GameplayTags",
			"GameplayTasks",
			"LevelSequence",
			"DataRegistry",
			
			//MM
			"MotionTrajectory",
			"MotionWarping",
			"PoseSearch",
			"AnimationWarpingRuntime",
			"IKRig",

			//other
			"Niagara",
			
			//network
			"OnlineSubsystem", 
			"OnlineSubsystemSteam", 
			"OnlineSubsystemUtils"

		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			// 通用UI组件（如菜单、HUD）。
			"CommonUI",
				
			// 跨平台输入统一管理。
			"CommonInput",
				
			// 游戏设置
			"GameSettings",
				
			// 通用游戏逻辑（如玩家管理、游戏模式）。
			"CommonGame",
				
			// 用户账户与权限系统。
			"CommonUser",
				
			// 字幕
			//"GameSubtitles",
		});

		PublicIncludePaths.AddRange(new string[] {
			"Abyss",
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}

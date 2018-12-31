// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.IO;

public class DiplomaVr : ModuleRules
{
	public DiplomaVr(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

        PublicDependencyModuleNames.AddRange(new string[] { "ShaderCore", "RenderCore", "RHI", "RuntimeMeshComponent" });

        PrivateDependencyModuleNames.AddRange(new string[] { "ImageWrapper", "RenderCore" });
        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true

        if ((Target.Platform == UnrealTargetPlatform.Win64) || (Target.Platform == UnrealTargetPlatform.Win32))
        {
            

            // path to directory containing this Build.cs file
            string PathToProject = System.IO.Path.GetFullPath(System.IO.Path.Combine(ModuleDirectory, "../../"));
            // string PathToProject = Directory.GetParent(ModulePath).Parent.Parent.ToString();
            PublicIncludePaths.Add(Path.Combine(PathToProject, "Source\\DiplomaVr\\include"));

            string PlatformString = (Target.Platform == UnrealTargetPlatform.Win64) ? "Win64" : "Win32";
            string LibrariesPath = Path.Combine(PathToProject, "Binaries", PlatformString);
            PublicAdditionalLibraries.Add(Path.Combine(LibrariesPath, "assimp-vc140-mt.lib"));
        }
    }
}

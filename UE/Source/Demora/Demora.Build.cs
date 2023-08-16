// exp.dev free

using UnrealBuildTool;

public class Demora : ModuleRules
{
	public Demora(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore" });

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"HairStrandsCore",
			"Niagara",
			"AugmentedReality",
			"LiveLinkInterface",
			"Networking",
			"Sockets",
		});

        if (Target.bBuildEditor == true)
        {
            PrivateDependencyModuleNames.Add("UnrealEd");
        }
    }
}

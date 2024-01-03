EngineIncludeDir = {}
EngineIncludeDir["vulkan"] = "%{wks.location}/Engine/Vendor/vulkan/Include"
EngineIncludeDir["stb"]    = "%{wks.location}/Engine/Vendor/stb"

EngineLibraryDir = {}
EngineLibraryDir["vulkan"] = "%{wks.location}/Engine/Vendor/vulkan/Lib"

project "Engine"
    kind          "StaticLib"
    language      "C++"
    cppdialect    "C++17"

    targetdir ("%{wks.location}/Build/%{prj.name}")
    objdir    ("%{wks.location}/Build/%{prj.name}/Obj")

    files
	{
		"Source/**.hpp",
        "Source/**.cpp",

        "%{EngineIncludeDir.stb}/**.h",

        "%{EngineIncludeDir.vulkan}/glm/**.hpp",
        "%{EngineIncludeDir.vulkan}/glm/**.inl",

        "%{EngineIncludeDir.vulkan}/SDL2/**.h"
	}

    includedirs
	{
        "Source",

		"%{EngineIncludeDir.stb}",
        "%{EngineIncludeDir.vulkan}"
    }
    
    libdirs
    {
        "%{EngineLibraryDir.vulkan}"
    }

    links
    {
        "SDL2",
        "vulkan-1"
    }
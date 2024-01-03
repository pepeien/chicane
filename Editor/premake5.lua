project "Editor"
    kind          "ConsoleApp"
    language      "C++"
    cppdialect    "C++17"

    targetdir ("%{wks.location}/Build/%{prj.name}")
    objdir    ("%{wks.location}/Build/%{prj.name}/Obj")

    files
	{
		"Source/**.hpp",
        "Source/**.cpp"
	}

    includedirs
	{
        "Source",

        "%{wks.location}/Engine/Source",
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
        "vulkan-1",

        "Engine"
    }
    
    filter { "configurations:Debug" }
        defines "DEBUG"

        symbols "on"

        debugdir "%{wks.location}/Build/%{prj.name}"

    filter { "configurations:Release" }
        defines "NDEBUG"

        optimize "on"
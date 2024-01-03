workspace "Chicane"
    architecture "x86_64"

    startproject "Editor"

    configurations {
        "Debug",
        "Release"
    }

include "Engine/premake5.lua"
include "Editor/premake5.lua"
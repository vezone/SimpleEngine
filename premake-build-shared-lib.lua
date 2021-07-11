workspace "Script"
architecture "x64"
--startproject "EngineEditor"

configurations
{
  "Debug",
  "Release",
  "Dist"
}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "Script"
    location "Engine/src/Script"
    kind "SharedLib"
    language "C"
    staticruntime "on"
    buildoptions { "-std=c99" }
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin/Intermidiates/" .. outputdir .. "/%{prj.name}")

    files
    {
      "%{prj.location}/**.h",
      "%{prj.location}/**.c"
    }

    defines
    {
      "_CRT_SECURE_NO_WARNINGS"
    }

    includedirs
    {
      "%{prj.location}/src"
    }

    filter "system:linux"
      defines { "LINUX_PLATFORM" }

    filter "system:windows"
      defines { "WINDOWS_PLATFORM" }

    filter "configurations:Debug"
      defines { "SCRIPT_DEBUG" }
      symbols "On"

    filter "configurations:Release"
      defines { "SCRIPT_RELEASE" }
      optimize "On"
      buildoptions { "-O3" }

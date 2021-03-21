workspace "Engine"
architecture "x64"
startproject "EngineEditor"

configurations
{
  "Debug",
  "Release",
  "Dist"
}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include dirs
IncludeDirs = {}
IncludeDirs["GLFW"] = "Dependencies/GLFW/include"
IncludeDirs["GLAD"] = "Dependencies/glad/include"
IncludeDirs["CGLM"] = "Dependencies/CGLM/include"
IncludeDirs["CIMGUI"] = "Dependencies/cimgui/cimgui/src"

group "Dependencies"
include "Dependencies/GLFW"
include "Dependencies/glad"
include "Dependencies/cimgui"

project "Engine"
    location "Engine"
    kind "StaticLib"
    language "C"
    staticruntime "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin/Intermidiates/" .. outputdir .. "/%{prj.name}")

    files
    {
      "%{prj.name}/src/**.h",
      "%{prj.name}/src/**.c"
    }

    defines
    {
      "_CRT_SECURE_NO_WARNINGS",
      "GLFW_INCLUDE_NONE",
      "CIMGUI_DEFINE_ENUMS_AND_STRUCTS",
      "_GNU_SOURCE"
    }

    includedirs
    {
      "%{prj.name}/src",
      "%{IncludeDirs.GLFW}",
      "%{IncludeDirs.GLAD}",
      "%{IncludeDirs.CGLM}",
      "%{IncludeDirs.CIMGUI}"
    }

    filter "system:linux"
      defines
      {
	 "LINUX_PLATFORM"
      }

    filter "system:windows"
      defines
      {
	 "WINDOWS_PLATFORM"
      }

    filter "configurations:Debug"
    defines
    {
       "ENGINE_DEBUG"
    }
    symbols "On"

    filter "configurations:Release"
    defines
    {
       "ENGINE_RELEASE"
    }
    optimize "On"

    filter "configurations:Dist"
    defines
    {
       "ENGINE_DIST"
    }
    optimize "On"

project "EngineEditor"
    location "EngineEditor"
    kind "ConsoleApp"
    language "C"
    staticruntime "on"

    buildoptions
    {
      "-std=c99",
      "-O3"
    }

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin/Intermidiates/" .. outputdir .. "/%{prj.name}")

    files
    {
      "%{prj.name}/src/**.h",
      "%{prj.name}/src/**.c"
    }

    defines
    {
      "GLFW_INCLUDE_NONE",
      "CIMGUI_DEFINE_ENUMS_AND_STRUCTS",
      "_GNU_SOURCE"
    }

    includedirs
    {
      "Engine/src",
      "Dependencies",
      "%{IncludeDirs.GLFW}",
      "%{IncludeDirs.GLAD}",
      "%{IncludeDirs.CGLM}",
      "%{IncludeDirs.CIMGUI}"
    }

    links
    {
      "Engine",
      "glad",
      "GLFW",
      "cimgui",
      "stdc++",
      "GL", "GLU",
      "X11","dl",
      "Xinerama", "Xcursor", "m",
      "Xxf86vm", "Xrandr", "pthread", "Xi"
    }

    filter "configurations:Debug"
    defines
    {
       "ENGINE_DEBUG"
    }
    symbols "On"

    filter "configurations:Release"
    defines
    {
       "ENGINE_RELEASE"
    }
    optimize "On"

    filter "configurations:Dist"
    defines
    {
       "ENGINE_DIST"
    }
    optimize "On"

    filter "system:linux"
      defines
      {
	 "LINUX_PLATFORM"
      }

    filter "system:windows"
      defines
      {
	 "WINDOWS_PLATFORM"
      }

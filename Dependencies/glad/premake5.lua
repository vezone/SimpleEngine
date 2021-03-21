project "glad"
	kind "StaticLib"
	language "C"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin/Intermidiates" .. outputdir .. "/%{prj.name}")

	includedirs
	{
		"include"
	}

	files
	{
		"include/glad/glad.h",
		"include/KGR/khrplatform.h",
		"src/glad.c"
	}

	filter "system:linux"
		pic "On"
		buildoptions { "-std=c99" }
		systemversion "latest"
		staticruntime "On"

	filter "system:windows"
		buildoptions { "-std=c99" }
		systemversion "latest"
		staticruntime "On"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
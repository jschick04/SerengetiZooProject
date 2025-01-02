project "ZooEngine"
    kind "StaticLib"
    language "C++"
	cppdialect "C++20"
	staticruntime "Off"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "zepch.h"
    pchsource "src/zepch.cpp"

    files
    {
        "src/**.h",
        "src/**.cpp"
    }

    includedirs
    {
		"src",
        "lib/wil",
        "lib/WriteLine"
    }

    links { "lib/WriteLine/WriteLine.lib" }

    filter "system:windows"
        systemversion "latest"

        defines
        {
            "PLATFORM_WINDOWS",
            "BUILD_DLL"
        }

        postbuildcommands
        {
            ("{COPYDIR} %{cfg.buildtarget.relpath} %{wks.location}/bin/" .. outputdir .. "/ZooApp/"),
            ("{COPYFILE} lib/WriteLine/WriteLine.dll %{wks.location}/bin/" .. outputdir .. "/ZooApp/WriteLine.dll")
        }

    filter "configurations:Debug"
		defines "DEBUG"
		runtime "Debug"
        symbols "On"

    filter "configurations:Release"
		defines "RELEASE"
		runtime "Release"
        optimize "On"
		symbols "Off"

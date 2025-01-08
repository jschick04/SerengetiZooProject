project "ZooEngine"
    kind "StaticLib"
    language "C++"
	cppdialect "C++20"
	staticruntime "on"

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
        "%{IncludeDir.wil}",
        "%{IncludeDir.WriteLine}"
    }

    links { "lib/WriteLine/WriteLine.lib" }

    filter "system:windows"
        systemversion "latest"
        defines "PLATFORM_WINDOWS"

        postbuildcommands
        {
            ("{COPYDIR} %{cfg.buildtarget.relpath} \"%{wks.location}/bin/" .. outputdir .. "/ZooApp/\""),
            ("{COPYFILE} lib/WriteLine/WriteLine.dll \"%{wks.location}/bin/" .. outputdir .. "/ZooApp/\"")
        }

    filter "configurations:Debug"
		defines "DEBUG"
		runtime "Debug"
        symbols "on"

    filter "configurations:Release"
		defines "RELEASE"
		runtime "Release"
        optimize "on"
		symbols "off"

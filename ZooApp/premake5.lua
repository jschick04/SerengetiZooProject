project "ZooApp"
    kind "ConsoleApp"
    language "C++"
	cppdialect "C++20"
	staticruntime "Off"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "src/**.h",
        "src/**.cpp",
    }

    includedirs {
        "%{wks.location}/ZooEngine/src",
        "%{wks.location}/ZooEngine/lib/wil",
        "%{wks.location}/ZooEngine/lib/WriteLine"
    }

    links { "ZooEngine" }

    filter "system:windows"
        systemversion "latest"
        defines "PLATFORM_WINDOWS"

    filter "configurations:Debug"
		defines "DEBUG"
		runtime "Debug"
        symbols "On"

    filter "configurations:Release"
		defines "RELEASE"
		runtime "Release"
        optimize "On"
		symbols "Off"

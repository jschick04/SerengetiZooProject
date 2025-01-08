project "ZooApp"
    kind "ConsoleApp"
    language "C++"
	cppdialect "C++20"
	staticruntime "on"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "src/**.h",
        "src/**.cpp",
    }

    includedirs {
        "%{wks.location}/ZooEngine/src",
        "%{IncludeDir.wil}",
        "%{IncludeDir.WriteLine}"
    }

    links { "ZooEngine" }

    filter "system:windows"
        systemversion "latest"
        defines "PLATFORM_WINDOWS"

    filter "configurations:Debug"
		defines "DEBUG"
		runtime "Debug"
        symbols "on"

    filter "configurations:Release"
		defines "RELEASE"
		runtime "Release"
        optimize "on"
		symbols "off"

workspace "SerengetiZooProject"
    architecture "x64"
	startproject "ZooApp"

    configurations { "Debug", "Release" }

	filter "system:windows"
	buildoptions { "/EHsc", "/Zc:preprocessor", "/Zc:__cplusplus" }

outputdir = "%{cfg.buildcfg}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["wil"] = "%{wks.location}/ZooEngine/lib/wil"
IncludeDir["WriteLine"] = "%{wks.location}/ZooEngine/lib/WriteLine"

include "ZooApp"
include "ZooEngine"

workspace "SerengetiZooProject"
    architecture "x64"
	startproject "ZooApp"

    configurations { "Debug", "Release" }

	filter "system:windows"
	buildoptions { "/EHsc", "/Zc:preprocessor", "/Zc:__cplusplus" }

outputdir = "%{cfg.buildcfg}-%{cfg.architecture}"

include "ZooApp"
include "ZooEngine"

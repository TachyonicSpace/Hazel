include "./vendor/premake/premake_customization/solution_items.lua"
include "Dependicies.lua"

workspace "Hazel"
	architecture "x86_64"
	startproject "Hazel-Nut"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

	solution_items
	{
		".editorconfig"
	}
	
	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"


group "Dependencies"
	include "vendor/premake"
	include "Hazel/vendor/GLFW"
	include "Hazel/vendor/Glad"
	include "Hazel/vendor/imgui"
	include "Hazel/vendor/yaml-cpp"
group ""


include "Hazel"
include "Sandbox"
include "Hazel-nut"
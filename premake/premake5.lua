-- premake5.lua
--[[
Usage examples: 
	for windows: premake5.exe --os=windows vs2015
	fot linux:   premake5.exe --os=linux gmake
]]

workspace "grpc_cb_core"
	location (_ACTION)  -- subdir vs2015 (or gmake, ...)
	configurations { "Release", "Debug" }
	platforms { "x64", "x32" }
	language "C++"
	flags {
		"C++11",
		-- "StaticRuntime",
	}

	require("conanpremake_multi")  -- for third-party libs

	includedirs {
		"../include",
	}

	filter "configurations:Debug"
		flags { "Symbols" }
	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"
	filter "system:windows"
		defines {
			"_WIN32_WINNT=0x0600"  -- i.e. Windows 7 target
		}
		links {
			"ws2_32",
		}
	filter {}

project "grpc_cb_core"
	kind "StaticLib"
	includedirs {
		"../src/cpp_cb",
	}
	files {
		"../include/grpc_cb/**.h",
		"../src/**",
		"../doc/**",
		"../*.md",
		"../*.py",
		"../*.txt",
	}

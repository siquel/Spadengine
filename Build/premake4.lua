solution "Spadengine"
	configurations { "DEBUG", "RELEASE" }

	configuration "Debug"
		defines { "DEBUG" }
		flags   { "Symbols" }

	configuration "Release"
		defines { "NDEBUG","RELEASE_BUILD" }
		flags   { "Optimize" }

	project "Core"
		kind "StaticLib"
		language "C++"
		files { "../Core/**.cpp" }
		includedirs { "../Core/Include/",
			      "../ThirdParty/glm/include/" }

	project "glad"
		kind "StaticLib"
		language "C"
		files {"../ThirdParty/glad/**.c"}
		includedirs {"../ThirdParty/glad/Include/"}
	
	project "Renderer"
		kind "StaticLib"
		language "C++"
		files { "../Renderer/**.cpp" }
		linkoptions {"../Build/libglad.a"}
		includedirs { "../Renderer/Include/",
				"../ThirdParty/SDL/include/",
				"../ThirdParty/glad/Include/" }
		

	project "Sample"
		kind "ConsoleApp"
		language "C++"
		files {"../Samples/Sample/**.cpp"}
		includedirs {"../Core/Include/",
				"../Renderer/Include/",
				"../ThirdParty/SDL/include/",
				"../ThirdParty/glm/include/",
				"../ThirdParty/glad/Include/",}

		links { "SDL2", "Core", "Renderer", "glad", "dl" }
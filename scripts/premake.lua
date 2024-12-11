workspace "Luar3D"
    configurations {"Debug", "Release"}
    platforms {"Win64"}
    location "../temp/solution"

    filter {"platforms:Win64"}
        architecture "x86_64"
        toolset "msc-v143"
        system "Windows"

    filter {"configurations:Debug"}
        optimize "Off"
        defines {"DEBUG"}

    filter {"configurations:Release"}
        optimize "On"

project "app"
    kind "ConsoleApp"
    language "C++"
    targetdir "../build/%{cfg.platform}/%{cfg.buildcfg}"   
    files { "../src/test/**.h", "../src/test/**.cpp" }
    includedirs 
    {
        "../src/",
        "../external/glfw-3.4.bin.WIN64/include"
    }
    warnings "Default"
    flags {"FatalWarnings"}
    libdirs{"../external/glfw-3.4.bin.WIN64/lib-vc2022"}
    links {"d3d12", "dxgi", "dxguid", "glfw3", "world", "renderer"}

project "world"
    kind "StaticLib"
    language "C++"
    targetdir "../build/%{cfg.platform}/%{cfg.buildcfg}"   
    files {"../src/world/**.cpp", "../src/world/**.h"}
    includedirs {"../src/world"}
    warnings "Default"
    flags {"FatalWarnings"}

project "renderer"
    kind "StaticLib"
    language "C++"
    targetdir "../build/%{cfg.platform}/%{cfg.buildcfg}"   
    files {"../src/renderer/**.cpp", "../src/renderer/**.h"}
    includedirs {"../src/renderer"}
    warnings "Default"
    flags {"FatalWarnings"}
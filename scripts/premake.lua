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

    filter {"configurations:Release"}
        optimize "On"

project "TestApp"
    kind "ConsoleApp"
    language "C++"
    targetdir "../build/%{cfg.platform}/%{cfg.buildcfg}"   
    files { "../src/**.h", "../src/**.cpp" }
    warnings "Everything"
    flags {"FatalWarnings"}
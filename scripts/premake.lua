require "emscripten"

workspace "Luar3D"
    configurations {"Debug", "Release"}
    platforms {"Win64", "Wasm64"}
    location "../temp/solution"

    filter {"configurations:Debug"}
        optimize "Off"
    
    filter {"configurations:Release"}
        optimize "On"

    filter {"platforms:Win64"}
        architecture "x86_64"

    filter {"platforms:Wasm64"}
        architecture "wasm64"
        system "emscripten"

project "TestApp"
    kind "ConsoleApp"
    language "C++"
    targetdir "../build/%{cfg.platform}/%{cfg.buildcfg}"   
    files { "**.h", "**.cpp"}
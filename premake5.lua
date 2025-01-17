workspace "physim2D"
    configurations { "Debug", "Release" }
    architecture "x86_64"
    startproject "physim2D"
    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
    targetdir ("bin/" .. outputdir)
    objdir ("bin-int/" .. outputdir)

    -- Third party paths
    THIRD_PARTY = "third_party"
    SDL_DIR = "%{THIRD_PARTY}/SDL"
    SDL_IMAGE_DIR = "%{THIRD_PARTY}/SDL_image"
    SDL_TTF_DIR = "%{THIRD_PARTY}/SDL_ttf"
    VMA_DIR = "%{THIRD_PARTY}/VMA"
    VKBOOTSTRAP_DIR = "%{THIRD_PARTY}/vkbootstrap"
    GLM_DIR = "%{THIRD_PARTY}/glm"
    IMGUI_DIR = "%{THIRD_PARTY}/imgui"
    FMT_DIR = "%{THIRD_PARTY}/fmt"
    ENGINE_DIR = "engine"

    -- ImGui project
    project "imgui"
        kind "StaticLib"
        language "C++"
        cppdialect "C++20"
        staticruntime "on"

        files {
            "%{IMGUI_DIR}/imgui.cpp",
            "%{IMGUI_DIR}/imgui_demo.cpp",
            "%{IMGUI_DIR}/imgui_draw.cpp",
            "%{IMGUI_DIR}/imgui_tables.cpp",
            "%{IMGUI_DIR}/imgui_widgets.cpp",
            "%{IMGUI_DIR}/backends/imgui_impl_sdl3.cpp",
            "%{IMGUI_DIR}/backends/imgui_impl_vulkan.cpp"
        }

        includedirs {
            "%{IMGUI_DIR}",
            "%{SDL_DIR}",
            "$(VULKAN_SDK)/Include"
        }

        links {
            "%{SDL_DIR}/SDL3.lib"
        }

        filter "system:windows"
            systemversion "latest"

    -- Main project
    project "physim2D"
        kind "ConsoleApp"
        language "C++"
        cppdialect "C++20"
        staticruntime "on"

        files {
            "game/**.h",
            "game/**.cpp",
            "engine/**.h",
            "engine/**.hpp",
            "engine/**.c",
            "engine/**.cpp",
            -- VkBootstrap files integrated here
            "%{VKBOOTSTRAP_DIR}/VkBootstrap.h",
            "%{VKBOOTSTRAP_DIR}/VkBootstrap.cpp"
        }

        includedirs {
            "%{THIRD_PARTY}",
            "%{SDL_DIR}",
            "%{SDL_IMAGE_DIR}",
            "%{SDL_TTF_DIR}",
            "%{VMA_DIR}",
            "%{ENGINE_DIR}",
            "%{IMGUI_DIR}",
            "%{GLM_DIR}",
            "%{FMT_DIR}/include",
            "$(VULKAN_SDK)/Include"
        }

        links {
            "imgui",
            "%{SDL_DIR}/SDL3.lib",
            "%{SDL_IMAGE_DIR}/SDL3_image.lib",
            "%{SDL_TTF_DIR}/SDL3_ttf.lib",
            "%{FMT_DIR}/build/Release/fmt.lib",
            "$(VULKAN_SDK)/Lib/vulkan-1"
        }

        postbuildcommands {
            "{COPY} %{SDL_DIR}/SDL3.dll %{cfg.targetdir}",
            "{COPY} %{SDL_IMAGE_DIR}/SDL3_image.dll %{cfg.targetdir}",
            "{COPY} %{SDL_TTF_DIR}/SDL3_ttf.dll %{cfg.targetdir}"
        }

        filter "configurations:Debug"
            runtime "Debug"
            symbols "on"
            defines { "DEBUG" }

        filter "configurations:Release"
            runtime "Release"
            optimize "On"
            vectorextensions "AVX"
            defines { "NDEBUG" }
            flags { 
                "MultiProcessorCompile"
            }

        filter "system:windows"
            systemversion "latest"
            defines { "WINDOWS" }
            buildoptions { "/utf-8" }

        filter "system:linux"
            defines { "LINUX" }
            links { "stdc++" }
            buildoptions { "-march=native" }
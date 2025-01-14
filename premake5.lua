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
    ENGINE_DIR = "engine"

    project "physim2D"
        kind "ConsoleApp"
        language "C++"
        cppdialect "C++20"
        staticruntime "on"

        files {
            -- Main project files
            "game/**.h",
            "game/**.cpp",
            "engine/**.h",
            "engine/**.hpp",
            "engine/**.c",
            "engine/**.cpp",
            -- GLM files
            "%{GLM_DIR}/glm/**.hpp",
            "%{GLM_DIR}/glm/**.inl",
            -- VkBootstrap files
            "%{VKBOOTSTRAP_DIR}/VkBootstrap.h",
            "%{VKBOOTSTRAP_DIR}/VkBootstrap.cpp",
            -- ImGui files
            "%{IMGUI_DIR}/imgui.cpp",
            "%{IMGUI_DIR}/imgui_demo.cpp",
            "%{IMGUI_DIR}/imgui_draw.cpp",
            "%{IMGUI_DIR}/imgui_tables.cpp",
            "%{IMGUI_DIR}/imgui_widgets.cpp",
            "%{IMGUI_DIR}/backends/imgui_impl_sdl3.cpp",
            "%{IMGUI_DIR}/backends/imgui_impl_vulkan.cpp"
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
            "$(VULKAN_SDK)/Include"
        }

        links {
            "%{SDL_DIR}/SDL3.lib",
            "%{SDL_IMAGE_DIR}/SDL3_image.lib",
            "%{SDL_TTF_DIR}/SDL3_ttf.lib",
            "$(VULKAN_SDK)/Lib/vulkan-1"
        }

        -- Copy DLLs post-build
        postbuildcommands {
            "{COPY} %{SDL_DIR}/SDL3.dll %{cfg.targetdir}",
            "{COPY} %{SDL_IMAGE_DIR}/SDL3_image.dll %{cfg.targetdir}",
            "{COPY} %{SDL_TTF_DIR}/SDL3_ttf.dll %{cfg.targetdir}"
        }

        filter "configurations:Debug"
            runtime "Debug"
            symbols "on"

        filter "configurations:Release"
            runtime "Release"
            optimize "on"
            vectorextensions "AVX"

        filter "system:windows"
            systemversion "latest"
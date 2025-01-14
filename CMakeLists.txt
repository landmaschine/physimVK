cmake_minimum_required(VERSION 3.14)
project(physim2D)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

set(THIRD_PARTY ${CMAKE_SOURCE_DIR}/third_party)
set(SDL_DIR ${THIRD_PARTY}/SDL)
set(SDL_IMAGE_DIR ${THIRD_PARTY}/SDL_image)
set(SDL_TTF_DIR ${THIRD_PARTY}/SDL_ttf)
set(VMA_DIR ${THIRD_PARTY}/VMA)
set(VKBOOTSTRAP_DIR ${THIRD_PARTY}/vkbootstrap)
set(GLM_DIR ${THIRD_PARTY}/glm)
set(IMGUI_DIR ${THIRD_PARTY}/imgui)
set(ENGINE_DIR ${CMAKE_SOURCE_DIR}/engine)

find_package(Vulkan REQUIRED)

if(MSVC)

else()
    add_compile_options(-O3 -march=native -mavx)
endif()

add_subdirectory(${GLM_DIR})

add_library(vkbootstrap STATIC
    ${VKBOOTSTRAP_DIR}/VkBootstrap.h
    ${VKBOOTSTRAP_DIR}/VkBootstrap.cpp
)
target_include_directories(vkbootstrap PUBLIC ${THIRD_PARTY})
target_link_libraries(vkbootstrap PUBLIC Vulkan::Vulkan)

add_library(imgui STATIC
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_sdl3.cpp
    ${IMGUI_DIR}/backends/imgui_impl_vulkan.cpp
)

target_include_directories(imgui PUBLIC
    ${IMGUI_DIR}
    ${SDL_DIR}
    ${Vulkan_INCLUDE_DIRS}
)

target_link_libraries(imgui PUBLIC
    ${SDL_DIR}/SDL3.lib
    Vulkan::Vulkan
)

file(GLOB GAME_SOURCES ${CMAKE_SOURCE_DIR}/game/*.cpp ${CMAKE_SOURCE_DIR}/game/*.h)
file(GLOB_RECURSE ENGINE_SOURCES
    ${CMAKE_SOURCE_DIR}/engine/*.c
    ${CMAKE_SOURCE_DIR}/engine/*.cpp
    ${CMAKE_SOURCE_DIR}/engine/*.h
    ${CMAKE_SOURCE_DIR}/engine/*.hpp
)
set(SOURCES ${GAME_SOURCES} ${ENGINE_SOURCES})

add_executable(physim2D ${SOURCES})

target_include_directories(physim2D PRIVATE
    ${THIRD_PARTY}
    ${SDL_DIR}
    ${SDL_IMAGE_DIR}
    ${SDL_TTF_DIR}
    ${VMA_DIR}
    ${ENGINE_DIR}
    ${Vulkan_INCLUDE_DIRS}
    ${IMGUI_DIR}
)

target_link_libraries(physim2D PRIVATE
    ${SDL_DIR}/SDL3.lib
    ${SDL_IMAGE_DIR}/SDL3_image.lib
    ${SDL_TTF_DIR}/SDL3_ttf.lib
    glm
    vkbootstrap
    Vulkan::Vulkan
    imgui
)

add_custom_command(TARGET physim2D POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${SDL_DIR}/SDL3.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    COMMAND ${CMAKE_COMMAND} -E copy ${SDL_IMAGE_DIR}/SDL3_image.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    COMMAND ${CMAKE_COMMAND} -E copy ${SDL_TTF_DIR}/SDL3_ttf.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)

cmake_minimum_required(VERSION 3.30)
project(sustainEngine VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# SDL3
set(SDL_SHARED ON CACHE BOOL "" FORCE)
set(SDL_STATIC OFF CACHE BOOL "" FORCE)
set(SDL_TEST OFF CACHE BOOL "" FORCE)
set(SDL_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(external/SDL)

# cimgui
include(cmake/GenerateCimguiBindings.cmake)
set(inclulist ${PROJECT_SOURCE_DIR}/external/SDL/include)
GenerateCimguiBindings(cimgui_with_backend sdl3 sdlgpu3 inclulist)
target_link_libraries(cimgui_with_backend PRIVATE SDL3::SDL3-shared)

# libsustain
add_library(sustain SHARED
    engine/src/core/window.c
    engine/src/core/gpu.c
    engine/src/core/log.c
    engine/src/renderer/renderer.c
)

target_include_directories(sustain
    PUBLIC
        ${PROJECT_SOURCE_DIR}/engine/include
    PRIVATE
        ${PROJECT_SOURCE_DIR}/external/SDL/include
)

target_link_libraries(sustain
    PUBLIC
        SDL3::SDL3-shared
)

# sustain editor
add_executable(sustain-editor
    editor/src/gui.c
    editor/src/main.c
)

target_include_directories(sustain-editor
    PRIVATE
        ${PROJECT_SOURCE_DIR}/editor/include
        ${PROJECT_SOURCE_DIR}/external/cimgui
        ${PROJECT_SOURCE_DIR}/external/cimgui/generator/output
)

target_compile_definitions(sustain-editor
    PRIVATE
        CIMGUI_DEFINE_ENUMS_AND_STRUCTS=1
        CIMGUI_USE_SDL3=1
        CIMGUI_USE_SDLGPU3=1
)

target_link_libraries(sustain-editor
    PRIVATE
        sustain
        cimgui_with_backend
        stdc++
        m
)

set_target_properties(sustain-editor PROPERTIES INSTALL_RPATH "$ORIGIN")

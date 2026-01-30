cmake_minimum_required(VERSION 3.5)
project(sustainEngine VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# SDL3
set(SDL_SHARED ON CACHE BOOL "" FORCE)
set(SDL_STATIC OFF CACHE BOOL "" FORCE)
set(SDL_TEST OFF CACHE BOOL "" FORCE)
set(SDL_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(external/SDL)

# libsustain
add_library(sustain SHARED
    engine/src/core/window.c
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
    editor/src/main.c
)

target_link_libraries(sustain-editor
    PRIVATE
        sustain
)

set_target_properties(sustain-editor PROPERTIES INSTALL_RPATH "$ORIGIN")

cmake_minimum_required(VERSION 3.16)
project(sustain LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(BUILD_SHARED_LIBS OFF)

# SDL3
set(SDL_SHARED          OFF CACHE BOOL "" FORCE)
set(SDL_STATIC          ON  CACHE BOOL "" FORCE)
set(SDL_TESTS           OFF CACHE BOOL "" FORCE)
if(UNIX AND NOT APPLE)
    set(SDL_WAYLAND      ON  CACHE BOOL "" FORCE)
endif()
set(SDL_X11_XSCRNSAVER  OFF CACHE BOOL "" FORCE)
set(SDL_X11_XTEST       OFF CACHE BOOL "" FORCE)
set(SDL_X11_XSYNC       OFF CACHE BOOL "" FORCE)
set(SDL_X11_XCURSOR     OFF CACHE BOOL "" FORCE)
set(SDL_X11_XDBE        OFF CACHE BOOL "" FORCE)
set(SDL_X11_XFIXES      OFF CACHE BOOL "" FORCE)
set(SDL_X11_XRANDR      OFF CACHE BOOL "" FORCE)
set(SDL_X11_XSHAPE      OFF CACHE BOOL "" FORCE)
set(SDL_INSTALL         OFF CACHE BOOL "" FORCE)
add_subdirectory(vendor/SDL)

# cglm
set(CGLM_STATIC ON  CACHE BOOL "" FORCE)
set(CGLM_SHARED OFF CACHE BOOL "" FORCE)
set(CGLM_USE_TEST OFF CACHE BOOL "" FORCE)
add_subdirectory(vendor/cglm)

# glad (generated GL 4.6 core loader, vendored)
add_subdirectory(vendor/glad-gen)

# flecs
set(FLECS_SHARED OFF CACHE BOOL "" FORCE)
set(FLECS_STATIC ON  CACHE BOOL "" FORCE)
add_subdirectory(vendor/flecs)

# microui
add_library(microui STATIC ${PROJECT_SOURCE_DIR}/vendor/microui/src/microui.c)
target_include_directories(microui PUBLIC ${PROJECT_SOURCE_DIR}/vendor/microui/src)

set(SHADER_OUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated)
add_custom_command(
    OUTPUT ${SHADER_OUT_DIR}/shaders_generated.c
           ${SHADER_OUT_DIR}/shaders_generated.h
    COMMAND ${CMAKE_COMMAND}
        -DOUT_HEADER=${SHADER_OUT_DIR}/shaders_generated.h
        -DOUT_SOURCE=${SHADER_OUT_DIR}/shaders_generated.c
        -DSHADER_ROOT=${PROJECT_SOURCE_DIR}/shaders
        -P ${PROJECT_SOURCE_DIR}/cmake/embed_shaders.cmake
    DEPENDS
        ${PROJECT_SOURCE_DIR}/shaders/primitive.vert
        ${PROJECT_SOURCE_DIR}/shaders/primitive.frag
        ${PROJECT_SOURCE_DIR}/shaders/grid.vert
        ${PROJECT_SOURCE_DIR}/shaders/grid.frag
        ${PROJECT_SOURCE_DIR}/shaders/gizmo.vert
        ${PROJECT_SOURCE_DIR}/shaders/gizmo.frag
        ${PROJECT_SOURCE_DIR}/shaders/sky.vert
        ${PROJECT_SOURCE_DIR}/shaders/sky.frag
        ${PROJECT_SOURCE_DIR}/shaders/blit.vert
        ${PROJECT_SOURCE_DIR}/shaders/blit.frag
        ${PROJECT_SOURCE_DIR}/shaders/ui.vert
        ${PROJECT_SOURCE_DIR}/shaders/ui.frag
    VERBATIM
)

set(SOURCES
    src/main.c
    src/core/core.c
    src/core/config.c
    src/core/input.c
    src/core/log.c
    src/math/math.c
    src/renderer/renderer.c
    src/renderer/editor_camera.c
    src/renderer/mesh.c
    src/renderer/shader.c
    src/ecs/components.c
    src/gui/microui_impl.c
    src/gui/gui.c
    src/gui/panel_hierarchy.c
    src/gui/panel_viewport.c
    src/gui/panel_properties.c
    ${SHADER_OUT_DIR}/shaders_generated.c
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${PROJECT_SOURCE_DIR}/inc
        ${SHADER_OUT_DIR}
)

target_include_directories(${PROJECT_NAME}
    SYSTEM PRIVATE
        ${PROJECT_SOURCE_DIR}/vendor/cglm/include
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        SDL3::SDL3-static
        cglm
        glad_gen
        flecs_static
        microui
)

find_program(CLANG_TIDY_EXE NAMES clang-tidy REQUIRED)
set_target_properties(${PROJECT_NAME} PROPERTIES
    C_CLANG_TIDY "${CLANG_TIDY_EXE};-p;${CMAKE_BINARY_DIR}"
)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE 
        /W4 
        /WX 
        /wd5287
        /wd4116
        /wd4244
        /wd4701
    )
    target_compile_definitions(${PROJECT_NAME} PRIVATE _CRT_SECURE_NO_WARNINGS)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE 
        -Wall 
        -Wextra 
        -Wpedantic 
        -Werror 
        -Wno-missing-field-initializers 
        -Wno-c23-extensions
    )
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE _POSIX_C_SOURCE=200809L)

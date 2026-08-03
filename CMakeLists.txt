cmake_minimum_required(VERSION 3.16)
project(sustain LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(BUILD_SHARED_LIBS OFF)

# raylib
set(USE_WAYLAND          OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_WAYLAND   OFF CACHE BOOL "" FORCE)
add_subdirectory(vendor/raylib)

# raylib nuklear
set(RAYLIB_NUKLEAR_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(vendor/raylib-nuklear)

# flecs
set(FLECS_SHARED OFF CACHE BOOL "" FORCE)
set(FLECS_STATIC ON CACHE BOOL "" FORCE)
add_subdirectory(vendor/flecs)

set(SOURCES
    src/main.c
    src/core/core.c
    src/core/config.c
    src/core/log.c
    src/renderer/renderer.c
    src/renderer/editor_camera.c
    src/ecs/components.c
    src/gui/nuklear_impl.c
    src/gui/gui.c
    src/gui/panel_hierarchy.c
    src/gui/panel_viewport.c
    src/gui/panel_properties.c
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${PROJECT_SOURCE_DIR}/inc
        ${PROJECT_SOURCE_DIR}/vendor/raylib/src
        ${PROJECT_SOURCE_DIR}/vendor/raylib-nuklear/include
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        raylib
        raylib_nuklear
        flecs_static
)

if(NOT WIN32)
  target_link_libraries(${PROJECT_NAME} PRIVATE m)
  target_link_options(${PROJECT_NAME} PRIVATE -Wl,--export-dynamic)
endif()

find_program(CLANG_TIDY_EXE NAMES clang-tidy REQUIRED)
set_target_properties(${PROJECT_NAME} PROPERTIES
    C_CLANG_TIDY "${CLANG_TIDY_EXE};-p;${CMAKE_BINARY_DIR}"
)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror -Wno-missing-field-initializers -Wno-c23-extensions)
target_compile_definitions(${PROJECT_NAME} PRIVATE _POSIX_C_SOURCE=200809L)

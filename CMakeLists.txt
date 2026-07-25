cmake_minimum_required(VERSION 3.16)
project(sustain LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(BUILD_SHARED_LIBS OFF)

set(USE_WAYLAND          OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_WAYLAND   OFF CACHE BOOL "" FORCE)
add_subdirectory(vendor/raylib)

set(RAYLIB_NUKLEAR_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(vendor/raylib-nuklear)

set(SOURCES
    src/main.c
    src/core.c
    src/config.c
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${PROJECT_SOURCE_DIR}/inc
        ${PROJECT_SOURCE_DIR}/vendor/cbase
        ${PROJECT_SOURCE_DIR}/vendor/raylib/src
        ${PROJECT_SOURCE_DIR}/vendor/raylib-nuklear/include
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        raylib
        raylib_nuklear
)

if(NOT WIN32)
  target_link_libraries(${PROJECT_NAME} PRIVATE m)
  target_link_options(${PROJECT_NAME} PRIVATE -Wl,--export-dynamic)
endif()

find_program(CLANG_TIDY_EXE NAMES clang-tidy REQUIRED)
set_target_properties(${PROJECT_NAME} PROPERTIES
    C_CLANG_TIDY "${CLANG_TIDY_EXE};-p;${CMAKE_BINARY_DIR}"
)

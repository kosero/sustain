cmake_minimum_required(VERSION 3.15)
project(SustainEngine VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CUSTOM_GLAD_REDEF ON)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-O3 -march=native -ffast-math -funroll-loops)
endif()

include(CheckIPOSupported)
check_ipo_supported(RESULT result OUTPUT output)
if(result)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
    message(WARNING "IPO is not supported: ${output}")
endif()

add_subdirectory(external/raylib)

file(GLOB_RECURSE SUSTAIN_SOURCES "sustain/src/*.c")
add_library(sustain SHARED ${SUSTAIN_SOURCES})

target_include_directories(sustain PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sustain/include>
    $<INSTALL_INTERFACE:include>
    external/raylib/src
)

target_link_libraries(sustain PUBLIC raylib)

add_executable(sustain_editor 
    editor/src/main.c
    editor/src/editor_state.c
    editor/src/editor_layout.c
    editor/src/gui_panels.c
)

target_include_directories(sustain_editor PRIVATE
    editor/include
    sustain/include
)

target_link_libraries(sustain_editor PRIVATE sustain)

add_custom_command(TARGET sustain_editor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E create_symlink
    ${CMAKE_CURRENT_SOURCE_DIR}/assets
    ${CMAKE_CURRENT_BINARY_DIR}/assets
)

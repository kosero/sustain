cmake_minimum_required(VERSION 3.5)
project(sustainEngine VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# SDL3
set(SDL_SHARED ON CACHE BOOL "" FORCE)
set(SDL_STATIC OFF CACHE BOOL "" FORCE)
set(SDL_TEST OFF CACHE BOOL "" FORCE)
set(SDL_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(external/SDL EXCLUDE_FROM_ALL)

# yaml
add_subdirectory(external/libyaml EXCLUDE_FROM_ALL)

# LuaJIT
set(LUA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/LuaJIT)
set(LUA_LIB ${LUA_DIR}/src/libluajit.a)
add_custom_command(OUTPUT ${LUA_LIB} COMMAND make -C ${LUA_DIR} -j$(nproc))
add_library(luajit_static STATIC IMPORTED GLOBAL)
set_target_properties(luajit_static PROPERTIES IMPORTED_LOCATION ${LUA_LIB})
add_custom_target(luajit_target DEPENDS ${LUA_LIB})

# libsustain
file(GLOB_RECURSE ENGINE_SRC "engine/src/*.c")
add_library(sustain SHARED ${ENGINE_SRC})
target_include_directories(sustain PUBLIC engine/include
    PRIVATE external/SDL/include external/libyaml/include)
target_link_libraries(sustain PUBLIC SDL3::SDL3-shared yaml)
set_target_properties(sustain PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# sustain editor
add_executable(sustain-editor editor/src/main.c)
target_include_directories(sustain-editor PRIVATE ${LUA_DIR}/src)
target_link_libraries(sustain-editor PRIVATE sustain luajit_static m dl)
add_dependencies(sustain-editor luajit_target)
set_target_properties(sustain-editor PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    INSTALL_RPATH "$ORIGIN")

file(GLOB LUA_FILES "editor/src/*.lua")
file(COPY ${LUA_FILES} DESTINATION ${CMAKE_BINARY_DIR}/bin)

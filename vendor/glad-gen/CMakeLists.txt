cmake_minimum_required(VERSION 3.16)
project(glad_gen LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_library(glad_gen STATIC src/gl.c)
target_include_directories(glad_gen PUBLIC include)
target_compile_options(glad_gen PRIVATE -w)

cmake_minimum_required (VERSION 3.22)

project(OPTIMIZATION)

file(MAKE_DIRECTORY bin)

set(SOURCE_OPT lab3_optimized.c)
set(SOURCE_ORG lab3_original.c)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pg -Wall -Werror -pedantic -Wextra -Wconversion -std=gnu11")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(lab3_optimized ${SOURCE_OPT})
add_executable(lab3_original ${SOURCE_ORG})
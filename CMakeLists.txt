cmake_minimum_required (VERSION 3.9.2)

# Project setup
project(mysh C)
set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)
set(VERSION_PATCH 0)

# TODO(CLang) support
add_compile_options(-Wall -Wextra -Wpedantic)

# TODO(Debug) treat debug and release differently
# add_compile_options()

add_subdirectory(src)

enable_testing()
add_subdirectory(test)

cmake_minimum_required(VERSION 3.0.0)
project(kivi-soft-renderer VERSION 0.1.0)

set(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}")
set(CMAKE_DEBUG_POSTFIX "_d")
set(CMAKE_RELEASE_POSTFIX "_r")
add_subdirectory(src)


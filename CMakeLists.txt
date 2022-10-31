cmake_minimum_required(VERSION 3.0.0)
project(kivi-soft-renderer VERSION 0.1.0)

add_executable(renderer main.cpp win32.cpp math.hpp pipeline.cpp model.cpp camera.cpp shader.cpp tgaimage.cpp)


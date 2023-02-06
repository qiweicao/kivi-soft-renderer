cmake_minimum_required(VERSION 3.0.0)

add_executable(renderer main.cpp win32.cpp pipeline.cpp model.cpp camera.cpp shader.cpp tgaimage.cpp)
set_target_properties(renderer PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
set_target_properties(renderer PROPERTIES RELEASE_POSTFIX ${CMAKE_RELEASE_POSTFIX})
install(TARGETS renderer DESTINATION bin)

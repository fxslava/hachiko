cmake_minimum_required(VERSION 3.16)
project(hachiko)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

add_subdirectory(test)
add_subdirectory(engine)
add_subdirectory(terrain_demo)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

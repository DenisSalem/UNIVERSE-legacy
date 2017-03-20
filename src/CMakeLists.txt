cmake_minimum_required (VERSION 3.7)

project (UNIVERSE)

add_subdirectory(gl3w) 

set(EXTERNAL_DEPENDENCIES "-lGL -ldl -lglfw")

add_executable(UNIVERSE main.cpp)

target_link_libraries (UNIVERSE gl3w ${EXTERNAL_DEPENDENCIES})


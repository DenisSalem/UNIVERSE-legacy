cmake_minimum_required (VERSION 3.7)

project (unit-tests)

include_directories("${PROJECT_SOURCE_DIR}/ProceduralEngine")
include_directories("${PROJECT_SOURCE_DIR}/gl3w")

set(EXTERNAL_DEPENDENCIES "-lGL -ldl -lglfw")

add_executable(unit-tests main.cpp)
target_link_libraries (unit-tests gl3w ${EXTERNAL_DEPENDENCIES})


cmake_minimum_required (VERSION 3.7)

project (UNIVERSE)

include_directories ("${PROJECT_SOURCE_DIR}/ProceduralEngine")
add_subdirectory(ProceduralEngine) 
add_subdirectory(gl3w) 
add_subdirectory(unit-tests) 

set(EXTERNAL_DEPENDENCIES "-lGL -ldl -lglfw")

#add_executable(UNIVERSE main.cpp)

#target_link_libraries (UNIVERSE gl3w ${EXTERNAL_DEPENDENCIES})


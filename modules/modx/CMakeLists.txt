cmake_minimum_required(VERSION 2.8)

project(modx)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
add_definitions(-DNUMA)
add_definitions(-D_GNU_SOURCE)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../module_def)

aux_source_directory(./src SRC_LIST) #all files in ./src will be considered source files

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../bin)


add_executable(modx ${SRC_LIST})

target_link_libraries(modx rt pthread numa dl nanomsg)


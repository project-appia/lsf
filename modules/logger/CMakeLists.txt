cmake_minimum_required(VERSION 2.8)

project(logger)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")


aux_source_directory(./src SRC_LIST) #all files in ./src will be considered source files

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../bin)


add_executable(logger ${SRC_LIST})

target_link_libraries(logger nanomsg)



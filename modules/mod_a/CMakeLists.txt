cmake_minimum_required(VERSION 2.8)


project(mod_a)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../module_def)

file(GLOB SOURCE_FILES "src/*.cpp")



set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../bin)


add_library(mod_a SHARED ${SOURCE_FILES})



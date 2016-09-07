project(lsf)
cmake_minimum_required(VERSION 2.8)
set(SRC_LIST
    ./main.cpp
    ./module.cpp
    ./module.h
    ./dispatcher.hpp)

set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")

include_directories("./lib/include")
if(WIN32)
link_directories("C:/git/lsf/lib/mingw491_32")
endif()

add_executable(${PROJECT_NAME} ${SRC_LIST})

target_link_libraries(${PROJECT_NAME} nanomsg)

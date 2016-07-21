project(lsf)
cmake_minimum_required(VERSION 2.8)
set(SRC_LIST
    ./main.cpp
    ./module.cpp
    ./module.h
   # ./module_A.cpp
   # ./module_A.h
   # ./module_B.cpp
   # ./module_B.h
    ./data.proto
    ./data.pb.h
    ./data.pb.cc
    ./dispatcher.hpp)

set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")

if(WIN32)
include_directories("C:/lib/libzmq/include"
                    "C:/lib/protobuf/src")
link_directories("C:/lib/libzmq/cmake-build/lib"
                 "C:/lib/protobuf/cmake-build")
endif()

add_executable(${PROJECT_NAME} ${SRC_LIST})

target_link_libraries(${PROJECT_NAME} zmq protobuf protoc gmock protobuf-lite)

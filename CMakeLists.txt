cmake_minimum_required(VERSION 3.10)

# set the project name and version
project(mgos-homie VERSION 1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
include_directories(include)
add_definitions(-DNO_MBEDTLS)

add_executable(test_prog 
    test-src/test.cpp src/homie.cpp 
    test-src/dtor.cpp test-src/net.cpp src/device.cpp 
    src/property.cpp src/node.cpp src/message.cpp)
target_link_libraries(test_prog stdc++)


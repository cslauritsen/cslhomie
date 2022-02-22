cmake_minimum_required(VERSION 3.14)

# set the project name and version
project(mgos-homie VERSION 1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
enable_testing()

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include_directories(include)
add_definitions(-DNO_MBEDTLS)
add_compile_options(-Wall -pedantic -Werror -Wextra -Oz)
add_executable(test_prog 
    test-src/test.cpp src/homie.cpp 
    test-src/dtor.cpp test-src/net.cpp src/device.cpp 
    src/property.cpp src/node.cpp src/message.cpp)
add_executable(suite
    test-src/suite.cpp src/homie.cpp 
    test-src/dtor.cpp test-src/net.cpp src/device.cpp 
    src/property.cpp src/node.cpp src/message.cpp)
target_link_libraries(suite stdc++ gtest_main)

include(GoogleTest)
gtest_discover_tests(suite)


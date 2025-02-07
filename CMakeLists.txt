cmake_minimum_required(VERSION 3.21)
project(LearningModernCpp)

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)  

# sub directories
add_subdirectory(coroutine)
add_subdirectory(asio_test)
add_subdirectory(container)
# add_subdirectory(qt_diy)
add_subdirectory(multithread)
add_subdirectory(modern_cpp)
add_subdirectory(cpp_syntax)

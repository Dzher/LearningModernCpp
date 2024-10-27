cmake_minimum_required(VERSION 3.21)
project(LearningModernCpp)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)  

# sub directories
add_subdirectory(coroutine)
add_subdirectory(asio_test)
add_subdirectory(container)

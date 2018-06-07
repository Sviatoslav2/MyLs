cmake_minimum_required(VERSION 3.5.1)
project(myls)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES main.cpp)
find_package(Boost COMPONENTS filesystem system REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
add_executable(myls ${SOURCE_FILES})
target_link_libraries( myls LINK_PUBLIC ${Boost_LIBRARIES} )
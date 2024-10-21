cmake_minimum_required(VERSION 3.29)
project(boost_alog)

find_package(Boost REQUIRED system)

set(CMAKE_CXX_STANDARD 17)

add_executable(alog main.cpp)

target_link_libraries(alog PUBLIC Boost::system)
target_include_directories(alog PUBLIC include)

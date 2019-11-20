cmake_minimum_required (VERSION 2.8.12)
project (rados_client)

if(CMAKE_COMPILER_IS_GNUCXX)
    add_compile_options(-std=c++11)
    message(STATUS "optional:-std=c++11")
endif(CMAKE_COMPILER_IS_GNUCXX)

add_executable(rados_client rados_client.cc)
target_link_libraries(rados_client rados)

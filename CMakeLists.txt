cmake_minimum_required (VERSION 2.8.12)
project (rbd_client)
SET(CMAKE_INSTALL_PREFIX /usr/local/bin/)

if(CMAKE_COMPILER_IS_GNUCXX)
    add_compile_options(-std=c++11)
    add_compile_options(-g)
    message(STATUS "optional:-std=c++11")
endif(CMAKE_COMPILER_IS_GNUCXX)

add_executable(rbd_client rbd_client.cc)
target_link_libraries(rbd_client rados rbd)

add_executable(rbd_compare_and_write rbd_compare_and_write.cc)
target_link_libraries(rbd_compare_and_write rados rbd)

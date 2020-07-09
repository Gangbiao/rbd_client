cmake_minimum_required (VERSION 2.8.12)
project (rbd_client)
SET(CMAKE_INSTALL_PREFIX /usr/local/bin/)

if(CMAKE_COMPILER_IS_GNUCXX)
    add_compile_options(-std=c++11)
    add_compile_options(-g)
    message(STATUS "optional:-std=c++11")
endif(CMAKE_COMPILER_IS_GNUCXX)
link_directories(/root/sdc/compile/ceph-14.2.9/build/lib/)
include_directories(/root/sdc/compile/ceph-14.2.9/src/include)

add_executable(rbd_client rbd_client.cc)
target_link_libraries(rbd_client rados rbd)

add_executable(rbd_compare_and_write rbd_compare_and_write.cc)
target_link_libraries(rbd_compare_and_write rados rbd)

add_executable(rbd_write_same rbd_write_same.cc)
target_link_libraries(rbd_write_same rados rbd)

add_executable(rbd_xcopy rbd_xcopy.cc)
target_link_libraries(rbd_xcopy rados rbd)

add_executable(rbd_create rbd_create.cc)
target_link_libraries(rbd_create rados rbd)

add_executable(rbd_zero rbd_zero.cc)
target_link_libraries(rbd_zero rados rbd)

add_executable(rbd_aio_zero rbd_aio_zero.cc)
target_link_libraries(rbd_aio_zero rados rbd)

add_executable(rbd_write rbd_write.cc)
target_link_libraries(rbd_write rados rbd)

add_executable(rbd_diff_iterate rbd_diff_iterate.cc)
target_link_libraries(rbd_diff_iterate rados rbd)

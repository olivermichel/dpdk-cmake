
cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_COMPILER /usr/local/bin/g++-9)

project(dpdk-test1 CXX)

include(cmake/dpdk.cmake)

add_executable(info info.cc)
target_include_directories(info PUBLIC ${DPDK_INCLUDE})
target_link_libraries(info pthread dl numa)
target_link_libraries(info ${DPDK_LIB})

add_executable(test1 test1.cc)
target_include_directories(test1 PUBLIC ${DPDK_INCLUDE})
target_link_libraries(test1 pthread dl numa)
target_link_libraries(test1 ${DPDK_LIB})


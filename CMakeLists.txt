
cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_COMPILER /usr/local/bin/g++-9)

project(dpdk-test1 CXX)

include(cmake/dpdk.cmake)
include(cmake/om2.cmake)

add_executable(info info.cc)
target_include_directories(info PUBLIC ${DPDK_INCLUDE})
target_include_directories(info PUBLIC ${CMAKE_HOME_DIRECTORY}/ext/include)
target_link_libraries(info pthread dl numa)
target_link_libraries(info ${DPDK_LIB})

add_executable(rx rx.cc)
target_include_directories(rx PUBLIC ${DPDK_INCLUDE})
target_include_directories(rx PUBLIC ${CMAKE_HOME_DIRECTORY}/ext/include)
target_link_libraries(rx pthread dl numa)
target_link_libraries(rx ${DPDK_LIB})

add_executable(tx tx.cc)
target_include_directories(tx PUBLIC ${DPDK_INCLUDE})
target_include_directories(tx PUBLIC ${CMAKE_HOME_DIRECTORY}/ext/include)
target_link_libraries(tx pthread dl numa)
target_link_libraries(tx ${DPDK_LIB})

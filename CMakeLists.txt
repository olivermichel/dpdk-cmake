
cmake_minimum_required(VERSION 2.6)

set(CMAKE_CXX_STANDARD 14)

project(dpdk-test1 CXX)

include(cmake/dpdk.cmake)

include_directories(PUBLIC ${CMAKE_HOME_DIRECTORY}/ext/include)

add_executable(info info.cc)
target_include_directories(info PUBLIC ${DPDK_INCLUDE})
target_link_libraries(info pthread dl numa)
target_link_libraries(info ${DPDK_LIB})

add_executable(rx rx.cc)
target_include_directories(rx PUBLIC ${DPDK_INCLUDE})
target_link_libraries(rx pthread dl numa)
target_link_libraries(rx ${DPDK_LIB})

add_executable(tx tx.cc)
target_include_directories(tx PUBLIC ${DPDK_INCLUDE})
target_link_libraries(tx pthread dl numa)
target_link_libraries(tx ${DPDK_LIB})

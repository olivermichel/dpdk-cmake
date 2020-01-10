
set(CXXOPTS_VERSION 2.2.0)

if(NOT EXISTS ${CMAKE_HOME_DIRECTORY}/ext/include/cxxopts/cxxopts.h)
    file(DOWNLOAD
            https://raw.githubusercontent.com/jarro2783/cxxopts/v${CXXOPTS_VERSION}/include/cxxopts.hpp
            ${CMAKE_HOME_DIRECTORY}/ext/include/cxxopts/cxxopts.h)
    message(STATUS "Downloading cxxpts: /ext/include/cxxopts/cxxopts.h - done")
endif()
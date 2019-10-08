## GENERAL PROJECT SETTINGS
cmake_minimum_required(VERSION 3.14)

project("eds-tools"
        VERSION 0.1
        LANGUAGES C CXX CUDA)

enable_testing()

include(CheckCXXCompilerFlag)
include(ExternalProject)
include(cmake/color_print.cmake)
include(FetchContent)


## PROJECT OPTIONS

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build, options are: Debug Release" FORCE)
    message(STATUS "Defaulting build type set to 'Debug'")
endif()
message(STATUS "Build type: ${Green}${CMAKE_BUILD_TYPE}${ColorReset}")

option(BUILD_SSE "Use SSE4.X instruction set to accelerate EDS functions" ON)
option(BUILD_AVX "Use AVX2 instruction set to accelerate EDS functions" ON)
option(BUILD_CUDA "Use CUDA instruction set to accelerate EDS functions" ON)

# If the user does not specify PREFIX, we default to ${CMAKE_BINARY_DIR} instead of system directory /usr
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "Default install path (prefix)" FORCE)
endif()


## COMPILERS

set(CMAKE_CXX_STANDARD 14)

set(FLAGS_WARN "-Wall -Wextra -Wno-unused-parameter -Wno-unused-variable")
set(CMAKE_C_FLAGS "${FLAGS_WARN}")
set(CMAKE_CXX_FLAGS "${FLAGS_WARN} -DBOOST_ERROR_CODE_HEADER_ONLY -DBOOST_SYSTEM_NO_DEPRECATED")
set(CMAKE_CUDA_FLAGS "--std c++14 --generate-code arch=compute_61,code=sm_61 --relocatable-device-code true --Werror cross-execution-space-call")

# Disable cudafe warning: "type qualifier is meaningless on cast type" occuring in boost::archive
# https://stackoverflow.com/questions/14831051/how-to-disable-compiler-warnings-with-nvcc
set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -Xcudafe --diag_suppress=cast_to_qualified_type,--diag_suppress=integer_sign_change,--diag_suppress=set_but_not_used")

## Boost 1.64 has a bug with Cuda 7.5 and 8.0, where compilation fails on static assert from g++ standard library
## https://svn.boost.org/trac10/ticket/13049
#set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -DBOOST_NO_CXX11_NOEXCEPT")

set(CMAKE_CUDA_FLAGS_DEBUG  "--debug --device-debug --generate-line-info")
set(CMAKE_CUDA_FLAGS_RELEASE "--optimize 3 --generate-line-info -DNDEBUG")
# set(CUDA_NVCC_FLAGS_RELWITHDEBUGINFO "--optimize 2 --generate-line-info")

set(CMAKE_CUDA_HOST_COMPILER ${CMAKE_CXX_COMPILER})


## DEPENDENCIES

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

### Boost

set(Boost_USE_STATIC_LIBS ON)
set(Boost_COMPONETNS program_options filesystem)

# WARNING: CMake may issue a warning: Imported targets not available for Boost version 106400
# This happens when CMake doesn't recognize too new version of Boost, for example CMake 3.7 doesn't recognize Boost
# 1.64, but CMake 3.8 already does. The warning causes missing Boost components dependencies, but may be ignored.
find_package(Boost 1.63.0 REQUIRED COMPONENTS ${Boost_COMPONETNS})

## Bzip2
find_package(BZip2 REQUIRED)


## googletest
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        release-1.10.0
)

## Global include directories
include_directories(${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/src ${Boost_INCLUDE_DIR})

## Global link directories
link_directories(${PROJECT_BINARY_DIR}/lib ${Boost_LIBRARY_DIR})

## Output destinations
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

add_subdirectory(src)
add_subdirectory(test)

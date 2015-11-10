PROJECT(rasta_irp CXX)
cmake_minimum_required(VERSION 2.8)

## Architecture set up
IF(UNIX)
  IF(APPLE)
    SET(GUI "Cocoa")
    cmake_policy(SET CMP0042 NEW)
  ELSE(APPLE)
    SET(GUI "X11")
  ENDIF(APPLE)
ELSE(UNIX)
  IF(WIN32)
    SET(GUI "Win32")
  ELSE(WIN32)
    SET(GUI "Unknown")
  ENDIF(WIN32)
ENDIF(UNIX)
MESSAGE("-- GUI system is ${GUI}")

if(CMAKE_GENERATOR MATCHES ".*(Make|Ninja).*"
    AND NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: Debug Release" FORCE)
  message(STATUS "CMAKE_BUILD_TYPE not specified. Using ${CMAKE_BUILD_TYPE} build")
endif()

################################################################################
#### VERSION SETTING

set(RASTA_IRP_VERSION_MAJOR 1)
set(RASTA_IRP_VERSION_MINOR 0)
set(RASTA_IRP_VERSION_PATCH 0)
set(RASTA_IRP_VERSION_STRING ${RASTA_IRP_VERSION_MAJOR}.${RASTA_IRP_VERSION_MINOR}.${RASTA_IRP_VERSION_PATCH})

################################################################################
#### HEADER FILE FOR CMAKE PARAMETERS

# configure a header file to pass some of the CMake settings to the source code
configure_file (
  "${PROJECT_SOURCE_DIR}/CMakeParams.h.in"
  "${PROJECT_BINARY_DIR}/CMakeParams.h"
)
# add the binary tree to the search path for include files so that we will find
# CmakeConfig.h

include_directories("${PROJECT_BINARY_DIR}")

# ################################################################################
# #### BOOST

# # Check if the Boost package is installed
# # set(Boost_NO_BOOST_CMAKE ON)
# # set(Boost_USE_MULTITHREADED OFF)
# find_package(Boost COMPONENTS log REQUIRED)
# find_package(Threads REQUIRED)

# # Addition of a Flag to link the boost logger Dynamicaly
# ADD_DEFINITIONS(-DBOOST_LOG_DYN_LINK)

# # Include of the Boost project header files
# include_directories(${Boost_INCLUDE_DIRS})

################################################################################
#### CPLEX
  

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake_extensions/)
find_package( Cplex )

# IF(CPLEX_FOUND)
#   SET(CPLEX_INCLUDE_DIRS ${CPLEX_INCLUDE_DIR})
#   SET(CPLEX_LIBRARIES ${CPLEX_LIBRARY};)
#   IF(CMAKE_SYSTEM_NAME STREQUAL "Linux")
#     SET(CPLEX_LIBRARIES "${CPLEX_LIBRARIES};m;pthread")
#   ENDIF(CMAKE_SYSTEM_NAME STREQUAL "Linux")
# ENDIF(CPLEX_FOUND)

IF(UNIX)
  IF(APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
  ELSE(APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pipe -fPIC")
  ENDIF(APPLE)
ENDIF(UNIX)

################################################################################
#### ADD LOGGER AND GOOGLE FLAGS

### Will be added if already installed !

# # add gflag
# add_subdirectory ("${CMAKE_CURRENT_SOURCE_DIR}/external/gflags")
# # add includes for gflags
# include_directories ("${gflags_BINARY_DIR}/include")
# # add library path for gflags
# set(CMAKE_LIBRARY_PATH "${gflags_BINARY_DIR}/lib" ${CMAKE_LIBRARY_PATH})

# add google-glog
add_subdirectory ("${CMAKE_CURRENT_SOURCE_DIR}/external/google-glog")
# add includes for google-glog
include_directories ("${google-glog_SOURCE_DIR}/src")
# add library path for google-glog
set(CMAKE_LIBRARY_PATH "${google-glog_BINARY_DIR}" ${CMAKE_LIBRARY_PATH})



################################################################################
#### ADD GOOGLE TEST

# add googletest
add_subdirectory ("${CMAKE_CURRENT_SOURCE_DIR}/external/googletest")
# add includes for googletest
include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/external/googletest/include")
# add library path for googletest
set(CMAKE_LIBRARY_PATH "${googletest_BINARY_DIR}" ${CMAKE_LIBRARY_PATH})



################################################################################
#### RASTA IRP LIBRARY

# set(CMAKE_CXX_FLAGS_DEBUG "-O0 -std=c++11 -ggdb -pipe -fPIC")
# set(CMAKE_CXX_FLAGS_RELEASE "-O3 -std=c++11 -pipe -fPIC")

# set(CMAKE_CXX_FLAGS_DEBUG "-g -std=c++11")
# set(CMAKE_CXX_FLAGS_RELEASE "-O3 -std=c++11")

# set(CMAKE_CXX_FLAGS_DEBUG "-g")
# set(CMAKE_CXX_FLAGS_RELEASE "-O3")

#### Use flags for c++11 and fix for maxOS

# try enable c++11
# g++ and clang++ need flags to enable c++11
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-std=c++11" HAS_CPP11)
if (HAS_CPP11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

# use libc++ on Mac OS X
# by default clang++ will use libstdc++ w/o c++11 support
if (APPLE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif ()

# Includes directory of sources to be able to find .h and .tpp of the project,
include_directories("${PROJECT_SOURCE_DIR}/src")

# Storage in variables of sources and includes files of the project
file(GLOB RIRP_SRC_FILES 
  "src/alg/*.cpp"
  "src/bo/*.cpp"
  "src/input/*.cpp"
  "src/output/*.cpp"
  "src/utils/*.cpp")

## Possibility of writting directly the parameter file !!! TODO
# configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/utils/datadir.cpp.in
#   ${CMAKE_CURRENT_BINARY_DIR}/datadir.cpp @ONLY)

### Create the library of the project

# Set the name of the library
set (PROJECT_LIB_NAME ${PROJECT_NAME})

# Create and link the library to the sources files
add_library(${PROJECT_LIB_NAME} SHARED ${RIRP_SRC_FILES})

# Link glog to the library
target_link_libraries(${PROJECT_LIB_NAME} glog)


# Set properties (optional)
set_target_properties(${PROJECT_LIB_NAME} 
  PROPERTIES VERSION ${RASTA_IRP_VERSION_STRING}
  SOVERSION ${RASTA_IRP_VERSION_MAJOR})


################################################################################
### RASTA IRP EXECUTABLE

add_executable(${PROJECT_NAME}_launcher src/main/main.cpp)
target_link_libraries(${PROJECT_NAME}_launcher ${PROJECT_LIB_NAME})


################################################################################
### RASTA IRP TESTS

option(BUILD_RASTA_IRP_TESTS "Build all of rasta irp tests." ON)

if (BUILD_RASTA_IRP_TESTS)
  add_executable(test_essais testing/essais.cpp)
  target_link_libraries(test_essais ${PROJECT_LIB_NAME} gtest_main)
  # cxx_executable(essais ../../../testing gtest_main ${PROJECT_LIB_NAME})
  # cxx_executable(sample2_unittest samples gtest_main samples/sample2.cc)
  # cxx_executable(sample3_unittest samples gtest_main)
  # cxx_executable(sample4_unittest samples gtest_main samples/sample4.cc)
  # cxx_executable(sample5_unittest samples gtest_main samples/sample1.cc)
  # cxx_executable(sample6_unittest samples gtest_main)
  # cxx_executable(sample7_unittest samples gtest_main)
  # cxx_executable(sample8_unittest samples gtest_main)
  # cxx_executable(sample9_unittest samples gtest)
  # cxx_executable(sample10_unittest samples gtest)
endif()

################################################################################
### DOCUMENTATION

option (GENERATE_DOCUMENTATION 
        "If you have Doxygen installed, you can generate documentation" ON)

IF (GENERATE_DOCUMENTATION)
  FIND_PACKAGE(Doxygen QUIET)
  IF (DOXYGEN_FOUND)
    SET(DOXYGEN_INPUT "${CMAKE_SOURCE_DIR}/Doxyfile")
    SET(DOXYGEN_OUTPUT "docs")

    ADD_CUSTOM_COMMAND(
        OUTPUT ${DOXYGEN_OUTPUT}
          COMMAND ${CMAKE_COMMAND} -E echo_append "Building API Documentation..."
          COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_INPUT}
          COMMAND ${CMAKE_COMMAND} -E echo "Done."
          WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
          DEPENDS ${DOXYGEN_INPUT}
    )

    ADD_CUSTOM_TARGET(apidoc ALL DEPENDS ${DOXYGEN_OUTPUT})

    ADD_CUSTOM_TARGET(apidoc_forced
    COMMAND ${CMAKE_COMMAND} -E echo_append "Building API Documentation..."
    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_INPUT}
    COMMAND ${CMAKE_COMMAND} -E echo "Done."
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

  ENDIF (DOXYGEN_FOUND)
ENDIF (GENERATE_DOCUMENTATION)




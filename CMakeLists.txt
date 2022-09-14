cmake_minimum_required(VERSION 3.16)

# Get version from git tag
include(cmake/version.cmake)
gitversion()

project(
  plihdf5
  LANGUAGES C CXX
  VERSION ${project_git_version})

if(NOT EXISTS ${CMAKE_BINARY_DIR}/CMakeCache.txt)
  if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE
        "Debug"
        CACHE STRING "" FORCE)
  endif()
endif()

if(NOT CMAKE_BUILD_TYPE STREQUAL "Release")
  message(WARNING "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
endif()

# Enable the usage of conan if wanted
if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
  include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
  conan_basic_setup()
endif()

# HDF5 Support
set(HDF5_PREFER_PARALLEL ON)
find_package(HDF5 REQUIRED COMPONENTS C HL)
if(NOT HDF5_IS_PARALLEL)
  message(FATAL_ERROR "HDF5 is not parallel")
endif()

find_package(MPI REQUIRED COMPONENTS C CXX)
find_package(OpenSSL REQUIRED)
find_package(nlohmann_json REQUIRED)

# Source files / headers
add_subdirectory(files)
add_subdirectory(src)

# Packaging
include(cmake/configure_files.cmake)
include(cmake/cpack_config.cmake)
include(cmake/install.cmake)

# Testing
option(BUILD_TESTING "Enable testing" ON)
if(BUILD_TESTING)
  if(NOT WIN32)
    enable_testing()
    add_subdirectory(test)
  endif()
endif()

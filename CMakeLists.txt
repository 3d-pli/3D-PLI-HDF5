cmake_minimum_required(VERSION 3.16)
project(plihdf5 LANGUAGES C CXX VERSION 1.0.1)

# Set some CMake standards
set(default_build_type "Release")
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Offer the user the choice of overriding the installation directories
set(INSTALL_LIB_DIR lib CACHE PATH "Installation directory for libraries")
set(INSTALL_BIN_DIR bin CACHE PATH "Installation directory for executables")
set(INSTALL_INCLUDE_DIR include CACHE PATH
  "Installation directory for header files")
set(INSTALL_SHARE_DIR share CACHE PATH
  "Installation directory for data files")
if(WIN32 AND NOT CYGWIN)
  set(DEF_INSTALL_CMAKE_DIR CMake)
else()
  set(DEF_INSTALL_CMAKE_DIR lib/cmake/PLIHDF5)
endif()
set(INSTALL_CMAKE_DIR ${DEF_INSTALL_CMAKE_DIR} CACHE PATH
  "Installation directory for CMake files")

# HDF5 Support
set(HDF5_PREFER_PARALLEL ON)
find_package(HDF5 REQUIRED COMPONENTS C HL)
include_directories(${HDF5_INCLUDE_DIRS})
if(NOT HDF5_IS_PARALLEL)
    message(FATAL_ERROR "HDF5 is not parallel")
endif()
set(PLIHDF5_LINK_LIBRARIES ${HDF5_LIBRARIES} ${HDF5_HL_LIBRARIES} )

# MPI Support
find_package(MPI REQUIRED COMPONENTS C CXX)
include_directories(${MPI_CXX_INCLUDE_DIRS} ${MPI_C_INCLUDE_DIRS})
set(PLIHDF5_LINK_LIBRARIES ${PLIHDF5_LINK_LIBRARIES} MPI::MPI_C MPI::MPI_CXX)

# OpenSSL Support
find_package(OpenSSL REQUIRED)
include_directories(${OpenSSL_INCLUDE_DIR})
set(PLIHDF5_LINK_LIBRARIES ${PLIHDF5_LINK_LIBRARIES} OpenSSL::SSL)

# JSON
find_package(nlohmann_json REQUIRED)
set(PLIHDF5_LINK_LIBRARIES ${PLIHDF5_LINK_LIBRARIES} nlohmann_json::nlohmann_json)

# Source files / headers
add_subdirectory(files)
include_directories(src)
add_subdirectory(src)

# Packaging
include(cmake/package.cmake)

# Testing
set(BUILD_TESTING ON CACHE BOOL "Enable testing")
if(BUILD_TESTING)
    if(NOT WIN32)
       enable_testing()
       add_subdirectory(test)
    endif()
endif()

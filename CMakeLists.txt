cmake_minimum_required(VERSION 3.16)
project(PLIHDF5 LANGUAGES C CXX VERSION 1.0.0)

# Set some CMake standards
set(default_build_type "Debug")
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
#set(JSON_BuildTests OFF CACHE INTERNAL "")
#add_subdirectory(extern/json)
#set(PLIHDF5_LINK_LIBRARIES ${PLIHDF5_LINK_LIBRARIES} nlohmann_json::nlohmann_json)

# Source files / headers
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
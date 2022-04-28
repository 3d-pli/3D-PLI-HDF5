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

# Testing
set(BUILD_TESTING ON CACHE BOOL "Enable testing")
if(BUILD_TESTING)
    if(NOT WIN32)
       enable_testing()
       add_subdirectory(test)
    endif()
endif()



###############################################################################
# BUILD PLATFORM
###############################################################################

find_program( LSB_RELEASE_EXEC lsb_release )

if( LSB_RELEASE_EXEC )
  execute_process( COMMAND ${LSB_RELEASE_EXEC} -is 
                   OUTPUT_VARIABLE LSB_RELEASE_ID_SHORT
                   OUTPUT_STRIP_TRAILING_WHITESPACE )
  execute_process( COMMAND ${LSB_RELEASE_EXEC} -rs 
                   OUTPUT_VARIABLE LSB_RELEASE_NUMBER
                   OUTPUT_STRIP_TRAILING_WHITESPACE )
  message( STATUS "Build platform : ${LSB_RELEASE_ID_SHORT}-${LSB_RELEASE_NUMBER}" )
  if( LSB_RELEASE_ID_SHORT STREQUAL "Ubuntu" )
    execute_process( COMMAND bash -c "dpkg --print-architecture"
                     COMMAND tr -d '\n' OUTPUT_VARIABLE outArch )
    if( LSB_RELEASE_NUMBER VERSION_GREATER 17.10 )
      set( DESIRED_QT_VERSION 5 )
      message( STATUS "Desired Qt version : ${DESIRED_QT_VERSION}")
    endif( LSB_RELEASE_NUMBER VERSION_GREATER 17.10 )
  else( LSB_RELEASE_ID_SHORT STREQUAL "Ubuntu" )
    set( outArch "amd64" )
  endif( LSB_RELEASE_ID_SHORT STREQUAL "Ubuntu" )
else( LSB_RELEASE_EXEC )
  set( LSB_RELEASE_ID_SHORT "Generic" )
  set( outArch "amd64" )
endif( LSB_RELEASE_EXEC )

message( STATUS "Build architecture : ${outArch}" )


set( CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}" )


include( InstallRequiredSystemLibraries )
set( CPACK_PACKAGE_VERSION_MAJOR "1" )
set( CPACK_PACKAGE_VERSION_MINOR "0" )
set( CPACK_PACKAGE_VERSION_PATCH "0" )

if ( LSB_RELEASE_ID_SHORT STREQUAL "Ubuntu" )

  set( CPACK_GENERATOR "DEB" )
  set( OS_NAME "" )
  set( CPACK_DEB_COMPONENT_INSTALL ON )
  set( CPACK_DEBIAN_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION}" )
  set( CPACK_DEBIAN_PACKAGE_ARCHITECTURE "${outArch}" )
  set( CPACK_DEBIAN_PACKAGE_NAME "${PROJECT_NAME}" )
  set( CPACK_DEBIAN_PACKAGE_MAINTAINER "Jan Reuter & Felix Matuschke" )
  set( CPACK_DEBIAN_PACKAGE_DESCRIPTION "3D PLI HDF5" )
  set( CPACK_DEBIAN_DEVEL_PACKAGE_DEPENDS "${PROJECT_NAME}" )
  set( CPACK_DEBIAN_DEVEL_PACKAGE_NAME "${PROJECT_NAME}-dev" )
  set( CPACK_DEBIAN_RUNTIME_PACKAGE_NAME "${PROJECT_NAME}" )

endif( LSB_RELEASE_ID_SHORT STREQUAL "Ubuntu" )


set( CPACK_PACKAGE_FILE_NAME 
     "${PROJECT_NAME}_${${PROJECT_NAME}_VERSION}${OS_NAME}_${outArch}" )
set( CPACK_COMPONENTS_ALL devel runtime )

include( CPack )


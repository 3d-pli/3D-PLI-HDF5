find_program( LSB_RELEASE_EXEC lsb_release )

##########
# Generate CMake files for install
##########
export(TARGETS PLIHDF5 FILE ${PROJECT_BINARY_DIR}/PLIHDF5Targets.cmake)
export(PACKAGE PLIHDF5)
# Create the PLIHDF5Config.cmake and PLIHDF5ConfigVersion files
file(RELATIVE_PATH REL_INCLUDE_DIR 
    "${CMAKE_INSTALL_PREFIX}/${INSTALL_CMAKE_DIR}"
    "${CMAKE_INSTALL_PREFIX}/${INSTALL_INCLUDE_DIR}")
# ... for the build tree
set(CONF_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}" "${PROJECT_BINARY_DIR}")
configure_file(cmake/PLIHDF5Config.cmake.in
  "${PROJECT_BINARY_DIR}/PLIHDF5Config.cmake" @ONLY)
# ... for the install tree
set(CONF_INCLUDE_DIRS "\${PLIHDF5_CMAKE_DIR}/${REL_INCLUDE_DIR}")
configure_file(cmake/PLIHDF5Config.cmake.in
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/PLIHDF5Config.cmake" @ONLY)
# ... for both
configure_file(cmake/PLIHDF5ConfigVersion.cmake.in
  "${PROJECT_BINARY_DIR}/PLIHDF5ConfigVersion.cmake" @ONLY)

# Install the PLIHDF5Config.cmake and FooBarConfigVersion.cmake
install(FILES
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/PLIHDF5Config.cmake"
  "${PROJECT_BINARY_DIR}/PLIHDF5ConfigVersion.cmake"
  DESTINATION "${INSTALL_CMAKE_DIR}" COMPONENT devel)

# Install the export set for use with the install-tree
install(EXPORT PLIHDF5Targets DESTINATION
"${INSTALL_CMAKE_DIR}" COMPONENT devel)

##########
# Create packages
##########
if( LSB_RELEASE_EXEC )
  # Get the OS release
  execute_process( COMMAND ${LSB_RELEASE_EXEC} -is 
                   OUTPUT_VARIABLE LSB_RELEASE_ID_SHORT
                   OUTPUT_STRIP_TRAILING_WHITESPACE )
  execute_process( COMMAND ${LSB_RELEASE_EXEC} -rs 
                   OUTPUT_VARIABLE LSB_RELEASE_NUMBER
                   OUTPUT_STRIP_TRAILING_WHITESPACE )
  message( STATUS "Build platform : ${LSB_RELEASE_ID_SHORT}-${LSB_RELEASE_NUMBER}" )

  # Get the system architecture
  EXECUTE_PROCESS( COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE ARCHITECTURE )
  # Convert x86_64 to amd64
  if( ${ARCHITECTURE} STREQUAL "x86_64" )
    SET( ARCHITECTURE amd64 )
  endif()
  message( STATUS "Build architecture : ${ARCHITECTURE}" )
endif( LSB_RELEASE_EXEC )

include( InstallRequiredSystemLibraries )
set( CPACK_PACKAGE_VERSION_MAJOR ${CMAKE_PROJECT_VERSION_MAJOR} )
set( CPACK_PACKAGE_VERSION_MINOR ${CMAKE_PROJECT_VERSION_MINOR} )
set( CPACK_PACKAGE_VERSION_PATCH ${CMAKE_PROJECT_VERSION_PATCH} )
set( CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")

if ( LSB_RELEASE_ID_SHORT STREQUAL "Ubuntu" )
  set( CPACK_GENERATOR "DEB" )
  set( OS_NAME "" )
  set( CPACK_DEB_COMPONENT_INSTALL ON )
  set( CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON )
  set( CPACK_DEBIAN_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION}" )
  set( CPACK_DEBIAN_PACKAGE_ARCHITECTURE "${ARCHITECTURE}" )
  set( CPACK_DEBIAN_PACKAGE_NAME "${PROJECT_NAME}" )
  set( CPACK_DEBIAN_PACKAGE_MAINTAINER "Jan Reuter & Felix Matuschke" )
  set( CPACK_DEBIAN_PACKAGE_DESCRIPTION "3D PLI HDF5" )
  set( CPACK_DEBIAN_RUNTIME_PACKAGE_NAME "${PROJECT_NAME}" )
  set( CPACK_DEBIAN_DEVEL_PACKAGE_DEPENDS "${PROJECT_NAME}" )
  set( CPACK_DEBIAN_DEVEL_PACKAGE_NAME "${PROJECT_NAME}-dev" )
else()
  set( CPACK_GENERATOR "STGZ" )
  set( OS_NAME "" )
  set( CPACK_STGZ_COMPRESSION "gzip" )
  set( CPACK_STGZ_COMPRESSION_LEVEL 9 )
  set( CPACK_STGZ_COMPRESSION_METHOD "DEFLATE" )
  set( CPACK_STGZ_COMPRESSION_TYPE "tar" )
  set( CPACK_STGZ_PACKAGE_NAME "${PROJECT_NAME}" )
  set( CPACK_STGZ_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION}" )
  set( CPACK_STGZ_PACKAGE_ARCHITECTURE "${ARCHITECTURE}" )
  set( CPACK_STGZ_PACKAGE_MAINTAINER "Jan Reuter & Felix Matuschke" )
  set( CPACK_STGZ_PACKAGE_DESCRIPTION "3D PLI HDF5" )
  set( CPACK_STGZ_DEVEL_PACKAGE_DEPENDS "${PROJECT_NAME}" )
  set( CPACK_STGZ_DEVEL_PACKAGE_NAME "${PROJECT_NAME}-dev" )
  set( CPACK_STGZ_RUNTIME_PACKAGE_NAME "${PROJECT_NAME}" )
endif()


set( CPACK_PACKAGE_FILE_NAME 
     "${PROJECT_NAME}_${${PROJECT_NAME}_VERSION}${OS_NAME}_${ARCHITECTURE}" )
set( CPACK_COMPONENTS_ALL devel runtime )
include( CPack )

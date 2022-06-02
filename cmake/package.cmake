include(cmake/package-config.cmake)

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


if ( LSB_RELEASE_ID_SHORT STREQUAL "Ubuntu" )
  set( CPACK_GENERATOR "DEB" )
else()
  set( CPACK_GENERATOR "STGZ" )
endif()


set( CPACK_PACKAGE_FILE_NAME 
     "${PROJECT_NAME}_${${PROJECT_NAME}_VERSION}${OS_NAME}_${ARCHITECTURE}" )
set( CPACK_COMPONENTS_ALL devel runtime )
include( CPack )

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# ##############################################################################
# Installation
# ##############################################################################
install(
  TARGETS PLIHDF5
  EXPORT PLIHDF5Targets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(DIRECTORY ${PROJECT_SOURCE_DIR}/src/PLIHDF5/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/PLIHDF5)

install(
  EXPORT PLIHDF5Targets
  FILE PLIHDF5Targets.cmake
  NAMESPACE PLIHDF5::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/PLIHDF5)

write_basic_package_version_file(
  "PLIHDF5ConfigVersion.cmake"
  VERSION ${CMAKE_PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion)

configure_package_config_file(
  ${PROJECT_SOURCE_DIR}/cmake/PLIHDF5Config.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/PLIHDF5Config.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/PLIHDF5)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/PLIHDF5Config.cmake"
              "${CMAKE_CURRENT_BINARY_DIR}/PLIHDF5ConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/PLIHDF5)

export(PACKAGE PLIHDF5)

#
# CPack
#
if(LSB_RELEASE_ID_SHORT STREQUAL "Ubuntu")
  set(CPACK_GENERATOR "DEB")
else()
  set(CPACK_GENERATOR "STGZ")
endif()

set(CPACK_PACKAGE_FILE_NAME
    "${PROJECT_NAME}_${${PROJECT_NAME}_VERSION}${OS_NAME}_${ARCHITECTURE}")
set(CPACK_COMPONENTS_ALL devel runtime)
include(CPack)

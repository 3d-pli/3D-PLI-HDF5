include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
include(GoogleTest)

configure_file(${PROJECT_SOURCE_DIR}/test/test_attributes.cpp.in
               ${PROJECT_SOURCE_DIR}/test/test_attributes.cpp)

file(GLOB files "test_*.cpp")
foreach(file ${files})
  get_filename_component(file_name ${file} NAME)
  string(REPLACE ".cpp" "" test_name "${file_name}")
  add_executable(${test_name} ${file})
  target_link_libraries(${test_name} PLIHDF5 ${PLIHDF5_LINK_LIBRARIES} gtest_main)
  set_property(TARGET ${test_name} PROPERTY CROSSCOMPILING_EMULATOR '${MPIEXEC_EXECUTABLE} --oversubscribe ${MPIEXEC_NUMPROC_FLAG} 4')
  gtest_discover_tests(${test_name})
endforeach()

add_executable(test_file_non_mpi no_mpi_test_file.cpp)
target_link_libraries(test_file_non_mpi PLIHDF5 ${PLIHDF5_LINK_LIBRARIES} gtest_main)
gtest_discover_tests(test_file_non_mpi)

add_executable(test_dataset_non_mpi no_mpi_test_dataset.cpp)
target_link_libraries(test_dataset_non_mpi PLIHDF5 ${PLIHDF5_LINK_LIBRARIES} gtest_main)
gtest_discover_tests(test_dataset_non_mpi)
include(FetchContent)

string(
  JOIN "/" _url
  https://github.com/google/googletest/archive/refs/tags/release-1.12.0.zip)

FetchContent_Declare(googletest URL ${_url})

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(GTEST_FORCE_SHARED_CRT
    ON
    CACHE BOOL "" FORCE)
set(INSTALL_GTEST
    OFF
    CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
include(GoogleTest)

configure_file(${PROJECT_SOURCE_DIR}/test/test_attributes.cpp.in
               ${PROJECT_SOURCE_DIR}/test/test_attributes.cpp)

file(GLOB files "test_*.cpp")

if(NOT DEFINED TEST_NUM_PROC)
  set(TEST_NUM_PROC "4")
endif()

foreach(file ${files})
  get_filename_component(file_name ${file} NAME)
  string(REPLACE ".cpp" "" test_name "${file_name}")
  add_executable(${test_name} ${file})
  target_link_libraries(${test_name} PLIHDF5 ${PLIHDF5_LINK_LIBRARIES}
                        gtest_main)
  set_property(
    TARGET ${test_name}
    PROPERTY CROSSCOMPILING_EMULATOR
             '${MPIEXEC_EXECUTABLE} --oversubscribe ${MPIEXEC_NUMPROC_FLAG}
       ${TEST_NUM_PROC}')
  gtest_discover_tests(${test_name})
endforeach()

add_executable(test_file_non_mpi no_mpi_test_file.cpp)
target_link_libraries(test_file_non_mpi PLIHDF5 ${PLIHDF5_LINK_LIBRARIES}
                      gtest_main)
gtest_discover_tests(test_file_non_mpi)

add_executable(test_dataset_non_mpi no_mpi_test_dataset.cpp)
target_link_libraries(test_dataset_non_mpi PLIHDF5 ${PLIHDF5_LINK_LIBRARIES}
                      gtest_main)
gtest_discover_tests(test_dataset_non_mpi)

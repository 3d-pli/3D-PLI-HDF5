include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
include(GoogleTest)

add_executable(test_attributes test_attributes.cpp)
target_link_libraries(test_attributes PLIHDF5 ${PLIHDF5_LINK_LIBRARIES} gtest_main)
gtest_discover_tests(test_attributes)

add_executable(test_dataset test_dataset.cpp)
target_link_libraries(test_dataset PLIHDF5 ${PLIHDF5_LINK_LIBRARIES} gtest_main)
gtest_discover_tests(test_dataset)

add_executable(test_file test_file.cpp)
target_link_libraries(test_file PLIHDF5 ${PLIHDF5_LINK_LIBRARIES} gtest_main)
gtest_discover_tests(test_file)

add_executable(test_group test_group.cpp)
target_link_libraries(test_group PLIHDF5 ${PLIHDF5_LINK_LIBRARIES} gtest_main)
gtest_discover_tests(test_group)

add_executable(test_link test_link.cpp)
target_link_libraries(test_link PLIHDF5 ${PLIHDF5_LINK_LIBRARIES} gtest_main)
gtest_discover_tests(test_link)

add_executable(test_type test_type.cpp)
target_link_libraries(test_type PLIHDF5 ${PLIHDF5_LINK_LIBRARIES} gtest_main)
gtest_discover_tests(test_type)

add_executable(test_plihdf5 test_plihdf5.cpp)
target_link_libraries(test_plihdf5 PLIHDF5 ${PLIHDF5_LINK_LIBRARIES} gtest_main)
gtest_discover_tests(test_plihdf5)

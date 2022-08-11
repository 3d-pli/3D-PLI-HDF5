add_library(
  PLIHDF5 SHARED
  plihdf5.cpp
  config.cpp
  file.cpp
  attributes.cpp
  group.cpp
  dataset.cpp
  link.cpp
  type.cpp
  sha512.cpp
  exceptions.cpp
  object.cpp)
add_library(PLIHDF5::PLIHDF5 ALIAS PLIHDF5)

target_compile_features(PLIHDF5 PUBLIC cxx_std_17 cxx_nullptr cxx_constexpr
                                       cxx_noexcept)
target_include_directories(
  PLIHDF5 PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>
                 $<INSTALL_INTERFACE:include/>)
target_link_libraries(
  PLIHDF5
  PUBLIC MPI::MPI_C MPI::MPI_CXX hdf5::hdf5 hdf5::hdf5_hl
  PRIVATE OpenSSL::SSL nlohmann_json::nlohmann_json)
if(MSVC)
  target_compile_options(PLIHDF5 PRIVATE /W4 /WX)
else()
  target_compile_options(
    PLIHDF5
    PRIVATE -Wall
            -Wextra
            -Wpedantic
            -Werror
            -Wfatal-errors
            -Wshadow
            -O)
endif()

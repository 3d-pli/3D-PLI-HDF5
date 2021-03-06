find_program(TOOLS_CLANG_TIDY NAMES clang-tidy-12 clang-tidy-11 clang-tidy-10
                                    clang-tidy)
find_program(TOOLS_CPPCHECK NAMES cppcheck)
find_program(TOOLS_CPPLINT NAMES cpplint)

option(PLIHDF5_USE_LINTING
       "Enable linting during compilation to check for code style." ON)

if(PLIHDF5_USE_LINTING)
  if(TOOLS_CPPCHECK)
    set(CMAKE_C_CPPCHECK
        ${TOOLS_CPPCHECK};
        -q;
        template=gcc;
        --error-exitcode=1;
        --enable=all;
        --suppress=unmatchedSuppression;
        --suppress=missingIncludeSystem;
        --suppress=unusedFunction;
        --suppress=preprocessorErrorDirective)
    set(CMAKE_CXX_CPPCHECK
        ${TOOLS_CPPCHECK};
        -q;
        template=gcc;
        --error-exitcode=1;
        --enable=all;
        --suppress=unmatchedSuppression;
        --suppress=missingIncludeSystem;
        --suppress=unusedFunction;
        --suppress=preprocessorErrorDirective)
  endif()

  if(TOOLS_CPPLINT)
    set(CMAKE_C_CPPLINT ${TOOLS_CPPLINT}; --verbose=1 --linelength=120)
    set(CMAKE_CXX_CPPLINT ${TOOLS_CPPLINT}; --verbose=1 --linelength=120)
  endif()

  if(TOOLS_CLANG_TIDY)
    set(CMAKE_C_CLANG_TIDY ${TOOLS_CLANG_TIDY})
    set(CMAKE_CXX_CLANG_TIDY ${TOOLS_CLANG_TIDY})
  endif()
endif()

configure_file(${PROJECT_SOURCE_DIR}/src/PLIHDF5/config.h.in
               ${PROJECT_SOURCE_DIR}/src/PLIHDF5/config.h)
set(PLIHDF5_HEADERS
    PLIHDF5/attributes.h
    PLIHDF5/attributes.tpp
    PLIHDF5/dataset.h
    PLIHDF5/dataset.tpp
    PLIHDF5/exceptions.h
    PLIHDF5/file.h
    PLIHDF5/group.h
    PLIHDF5/link.h
    PLIHDF5/plihdf5.h
    PLIHDF5/sha512.h
    PLIHDF5/type.h
    PLIHDF5/config.h
    PLIHDF5/object.h)

set(PLIHDF5_SOURCES
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

add_library(PLIHDF5 SHARED ${PLIHDF5_SOURCES})

if(MSVC)
  target_compile_options(PLIHDF5 PRIVATE /W4 /WX)
else()
  target_compile_options(PLIHDF5 PRIVATE -Wall -Wextra -Wpedantic -Werror
                                         -Wshadow -O)
endif()

target_compile_features(
  PLIHDF5 PRIVATE cxx_std_17 cxx_constexpr cxx_defaulted_functions
                  cxx_deleted_functions cxx_noexcept)
target_link_libraries(PLIHDF5 ${PLIHDF5_LINK_LIBRARIES})

# ##############################################################################
# Installation
# ##############################################################################
install(
  DIRECTORY "PLIHDF5" EXPORT PLIHDF5Targets
  DESTINATION "${INSTALL_INCLUDE_DIR}"
  COMPONENT devel
  FILES_MATCHING
  PATTERN "*.h")
install(
  DIRECTORY "PLIHDF5" EXPORT PLIHDF5Targets
  DESTINATION "${INSTALL_INCLUDE_DIR}"
  COMPONENT devel
  FILES_MATCHING
  PATTERN "*.tpp")
install(
  TARGETS PLIHDF5
  EXPORT PLIHDF5Targets
  DESTINATION "${INSTALL_LIB_DIR}"
  COMPONENT runtime)

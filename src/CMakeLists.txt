option(PLIHDF5_USE_LINTING "Enable linting during compilation to check for code style." ON)
if(PLIHDF5_USE_LINTING)
    set(CMAKE_C_CPPCHECK cppcheck; -q; template=gcc; --error-exitcode=1; --enable=all; --suppress=unmatchedSuppression; --suppress=missingIncludeSystem; --suppress=unusedFunction)
    set(CMAKE_CXX_CPPCHECK cppcheck; -q; template=gcc; --error-exitcode=1; --enable=all; --suppress=unmatchedSuppression; --suppress=missingIncludeSystem; --suppress=unusedFunction)
    set(CMAKE_C_CPPLINT cpplint; --verbose=1)
    set(CMAKE_CXX_CPPLINT cpplint; --verbose=1)
    set(CMAKE_C_CLANG_TIDY clang-tidy)
    set(CMAKE_CXX_CLANG_TIDY clang-tidy)
endif()

set(PLIHDF5_SOURCES
        plihdf5.cpp
        file.cpp
        attributes.cpp
        group.cpp
        dataset.cpp
        link.cpp
        type.cpp
        sha256.cpp
    )

add_library(PLIHDF5 SHARED ${PLIHDF5_SOURCES})
if(MSVC)
  target_compile_options(PLIHDF5 PRIVATE /W4 /WX)
else()
  target_compile_options(PLIHDF5 PRIVATE -Wall -Wextra -Wpedantic -Werror -O)
endif()
target_link_libraries(PLIHDF5 ${PLIHDF5_LINK_LIBRARIES})


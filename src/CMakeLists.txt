option(PLIHDF5_USE_LINTING "Enable linting during compilation to check for code style." ON)
if(PLIHDF5_USE_LINTING)
    set(CMAKE_C_CPPCHECK cppcheck; -q; template=gcc; --error-exitcode=1; --enable=all; --suppress=unmatchedSuppression; --suppress=missingIncludeSystem; --suppress=unusedFunction)
    set(CMAKE_CXX_CPPCHECK cppcheck; -q; template=gcc; --error-exitcode=1; --enable=all; --suppress=unmatchedSuppression; --suppress=missingIncludeSystem; --suppress=unusedFunction)
    set(CMAKE_C_CPPLINT cpplint; --verbose=1)
    set(CMAKE_CXX_CPPLINT cpplint; --verbose=1)
    set(CMAKE_C_CLANG_TIDY clang-tidy)
    set(CMAKE_CXX_CLANG_TIDY clang-tidy)
endif()


set(PLIHDF5_HEADERS
    PLIHDF5/attributes.h
    PLIHDF5/attributes.tpp
    PLIHDF5/dataset.h
    PLIHDF5/exceptions.h
    PLIHDF5/file.h
    PLIHDF5/group.h
    PLIHDF5/link.h
    PLIHDF5/plihdf5.h
    PLIHDF5/sha256.h
    PLIHDF5/type.h
   )


set(PLIHDF5_SOURCES
        plihdf5.cpp
        file.cpp
        attributes.cpp
        group.cpp
        dataset.cpp
        link.cpp
        type.cpp
        sha256.cpp
        exceptions.cpp
    )

add_library(PLIHDF5 SHARED ${PLIHDF5_SOURCES})
if(MSVC)
  target_compile_options(PLIHDF5 PRIVATE /W4 /WX)
else()
  target_compile_options(PLIHDF5 PRIVATE -Wall -Wextra -Wpedantic -Werror -Wshadow -O)
endif()
target_link_libraries(PLIHDF5 ${PLIHDF5_LINK_LIBRARIES} )



################################################################################
# Installation
################################################################################

install( DIRECTORY "PLIHDF5"
         DESTINATION include
         COMPONENT devel
         FILES_MATCHING PATTERN "*.h" )
install( DIRECTORY "PLIHDF5"
         DESTINATION include
         COMPONENT devel
         FILES_MATCHING PATTERN "*.tpp" )
install( TARGETS PLIHDF5 DESTINATION lib COMPONENT runtime )

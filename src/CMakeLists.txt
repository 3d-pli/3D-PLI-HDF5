set(PLIHDF5_SOURCES
        plihdf5.cpp
        file.cpp
        attributes.cpp
        group.cpp
        dataset.cpp
        link.cpp
    )

add_library(PLIHDF5 SHARED ${PLIHDF5_SOURCES})
target_link_libraries(PLIHDF5 ${PLIHDF5_LINK_LIBRARIES})

##################### Files #######################
file(
    GLOB_RECURSE
    HEADERS

    # Sources
    ${GRID_HEADER_DIR}/*.hpp
)

file(
    GLOB_RECURSE
    SOURCES

    # Sources
    ${GRID_SOURCE_DIR}/*.cpp
)

##################### Setup #######################
add_library(${GRID_TARGET_NAME} ${HEADERS} ${SOURCES})

add_library(
    "${NAME}::Grid"

    ALIAS
        ${GRID_TARGET_NAME}
)

target_include_directories(
    ${GRID_TARGET_NAME}

    PUBLIC
        # Alias
        $<BUILD_INTERFACE:${BUILD_INTERFACE_DIR}>
        $<INSTALL_INTERFACE:${INSTALL_INTERFACE_DIR}>

    PRIVATE
        # Systems
        ${GRID_INCLUDE_DIRS}
)

target_link_libraries(
    ${GRID_TARGET_NAME}

    PRIVATE
        # Systems
        ${GRID_LIBRARIES}
)

CH_ADD_COMPILER(${GRID_TARGET_NAME})
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

set_target_properties(
    ${GRID_TARGET_NAME}

    PROPERTIES
        OUTPUT_NAME
            "${NAME}Grid"
        VS_DEBUGGER_WORKING_DIRECTORY
            "${BUILD_DIR}"
)

target_include_directories(
    ${GRID_TARGET_NAME}

    PUBLIC
        # Alias
        $<BUILD_INTERFACE:${BUILD_INTERFACE_DIR}>
        $<INSTALL_INTERFACE:${INSTALL_INTERFACE_DIR}>
)

target_link_libraries(
    ${GRID_TARGET_NAME}

    PUBLIC
        # Systems
        Chicane::Box
        Chicane::Core
)

CH_ADD_COMPILER(${GRID_TARGET_NAME})
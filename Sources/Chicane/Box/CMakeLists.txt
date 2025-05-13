##################### Files #######################
file(
    GLOB_RECURSE
    SOURCES

    # Sources
    ${BOX_SOURCE_DIR}/*.cpp
)

##################### Setup #######################
add_library(${BOX_TARGET_NAME} ${SOURCES})

add_library(
    "${NAME}::Box"

    ALIAS
        ${BOX_TARGET_NAME}
)

set_target_properties(
    ${BOX_TARGET_NAME}

    PROPERTIES
        OUTPUT_NAME
            "${NAME}Box"
        VS_DEBUGGER_WORKING_DIRECTORY
            "${BUILD_DIR}"
)

target_include_directories(
    ${BOX_TARGET_NAME}

    PUBLIC
        # Alias
        $<BUILD_INTERFACE:${BUILD_INTERFACE_DIR}>
        $<INSTALL_INTERFACE:${INSTALL_INTERFACE_DIR}>

    PUBLIC
        # Vendors
        ${VENDORS_DIR}/fast_obj
)

target_link_libraries(
    ${BOX_TARGET_NAME}

    PUBLIC
        # Systems
        Chicane::Core
)

CH_ADD_COMPILER(${BOX_TARGET_NAME})
######################### Files ##########################
file(
    GLOB_RECURSE
    HEADERS
    CONFIGURE_DEPENDS
    ${CHICANE_PROGRAMS_BOX_UTILS_INCLUDES_DIR}/*.hpp
)

file(
    GLOB_RECURSE
    SOURCES
    CONFIGURE_DEPENDS
    ${CHICANE_PROGRAMS_BOX_UTILS_SOURCES_DIR}/*.cpp
)

######################### Setup #########################
add_executable(${CHICANE_PROGRAMS_BOX_UTILS_TARGET_NAME} ${HEADERS} ${SOURCES})

set_target_properties(
    ${CHICANE_PROGRAMS_BOX_UTILS_TARGET_NAME}

    PROPERTIES
        OUTPUT_NAME
            "${NAME}BoxUtils"
        VS_DEBUGGER_WORKING_DIRECTORY
            "${CHICANE_BUILD_DIR}"
        POSITION_INDEPENDENT_CODE
            TRUE
)

target_include_directories(
    ${CHICANE_PROGRAMS_BOX_UTILS_TARGET_NAME}

    PUBLIC
        $<BUILD_INTERFACE:${CHICANE_BUILD_INTERFACE_DIR}>
        $<INSTALL_INTERFACE:${CHICANE_INSTALL_INTERFACE_DIR}>
    PRIVATE
        ${CHICANE_SOURCES_DIR}
)

target_link_libraries(
    ${CHICANE_PROGRAMS_BOX_UTILS_TARGET_NAME}

    PRIVATE
        Chicane::Box
)

CH_ADD_COMPILER(${CHICANE_PROGRAMS_BOX_UTILS_TARGET_NAME})
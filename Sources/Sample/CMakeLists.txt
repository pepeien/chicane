######################### Files ##########################
file(
    GLOB_RECURSE
    HEADERS
    CONFIGURE_DEPENDS
    ${CHICANE_SAMPLE_INCLUDES_DIR}/*.hpp
)

file(
    GLOB_RECURSE
    SOURCES
    CONFIGURE_DEPENDS
    ${CHICANE_SAMPLE_SOURCES_DIR}/*.cpp
)

##################### Directories #######################
add_executable(${CHICANE_SAMPLE_TARGET_NAME} ${HEADERS} ${SOURCES})

set_target_properties(
    ${CHICANE_SAMPLE_TARGET_NAME}

    PROPERTIES
        OUTPUT_NAME
            "${NAME}Sample"
        VS_DEBUGGER_WORKING_DIRECTORY
            "${CHICANE_BUILD_DIR}"
        CH_REFLECT_HEADERS
            "${HEADERS}"
        CH_BASE_HEADER_DIR
            "${CHICANE_SOURCES_DIR}"
        CH_SOURCE_HEADER_DIR
            "${CHICANE_SAMPLE_INCLUDES_DIR}"
)

target_include_directories(
    ${CHICANE_SAMPLE_TARGET_NAME}

    PUBLIC
        $<BUILD_INTERFACE:${CHICANE_BUILD_INTERFACE_DIR}>
        $<INSTALL_INTERFACE:${CHICANE_INSTALL_INTERFACE_DIR}>
    PRIVATE
        ${CHICANE_SOURCES_DIR}
)

target_link_libraries(
    ${CHICANE_SAMPLE_TARGET_NAME}

    PRIVATE
        Chicane::Runtime
)

CH_ADD_COMPILER(${CHICANE_SAMPLE_TARGET_NAME})

######################### Depedencies #########################
add_dependencies(${CHICANE_SAMPLE_TARGET_NAME} ${CHICANE_SAMPLE_ASSETS_TARGET_NAME})
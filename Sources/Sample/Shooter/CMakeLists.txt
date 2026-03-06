######################### Files ##########################
file(
    GLOB_RECURSE
    HEADERS

    ${SHOOTER_SOURCE_DIR}/*.hpp
)

file(
    GLOB_RECURSE
    SOURCES

    ${SHOOTER_SOURCE_DIR}/*.cpp
)

##################### Directories #######################
add_executable(${SHOOTER_TARGET_NAME} ${HEADERS} ${SOURCES})

set_target_properties(
    ${SHOOTER_TARGET_NAME}

    PROPERTIES
        OUTPUT_NAME
            "ShooterSample"
        VS_DEBUGGER_WORKING_DIRECTORY
            "${CHICANE_BUILD_DIR}"
)

target_include_directories(
    ${SHOOTER_TARGET_NAME}

    PUBLIC
        $<BUILD_INTERFACE:${BUILD_INTERFACE_DIR}>
        $<INSTALL_INTERFACE:${INSTALL_INTERFACE_DIR}>
    PRIVATE
        ${SOURCES_DIR}
)

target_link_libraries(
    ${SHOOTER_TARGET_NAME}

    PRIVATE
        Chicane::Runtime
)

add_dependencies(${SHOOTER_TARGET_NAME} ${CONTENTS_TARGET_NAME})

CH_ADD_COMPILER(${SHOOTER_TARGET_NAME})

CH_REFLECT_FILE(${SHOOTER_TARGET_NAME} "${HEADERS}" ${SOURCES_DIR} ${SHOOTER_SOURCE_DIR})
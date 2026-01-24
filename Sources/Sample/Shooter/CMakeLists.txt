######################### Files ##########################
file(
    GLOB_RECURSE
    HEADERS

    ${CMAKE_CURRENT_LIST_DIR}/*.hpp
)

file(
    GLOB_RECURSE
    SOURCES

    ${CMAKE_CURRENT_LIST_DIR}/*.cpp
)

##################### Directories #######################
add_executable(${SHOOTER_TARGET_NAME} ${HEADERS} ${SOURCES})

set_target_properties(
    ${SHOOTER_TARGET_NAME}

    PROPERTIES
        OUTPUT_NAME
            "ShooterSample"
        VS_DEBUGGER_WORKING_DIRECTORY
            "${BUILD_DIR}"
)

target_include_directories(
    ${SHOOTER_TARGET_NAME}

    PUBLIC
        $<BUILD_INTERFACE:${BUILD_INTERFACE_DIR}>
        $<INSTALL_INTERFACE:${INSTALL_INTERFACE_DIR}>
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(
    ${SHOOTER_TARGET_NAME}

    PRIVATE
        Chicane::Runtime
)

add_dependencies(${SHOOTER_TARGET_NAME} ${SHOOTER_CONTENT_TARGET_NAME})

CH_ADD_COMPILER(${SHOOTER_TARGET_NAME})
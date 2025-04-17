######################### Files ##########################
file(
    GLOB_RECURSE
    HEADERS

    ${SHOOTER_HEADER_DIR}/*.hpp
)

file(
    GLOB_RECURSE
    SOURCES

    ${SHOOTER_SOURCE_DIR}/*.cpp
)

##################### Directories #######################
add_executable(${SHOOTER_TARGET_NAME} ${HEADERS} ${SOURCES})

target_include_directories(
    ${SHOOTER_TARGET_NAME}

    PUBLIC
        $<BUILD_INTERFACE:${SHOOTER_BUILD_INTERFACE_DIR}>
        ${INCLUDES_DIR}
)

target_link_libraries(
    ${SHOOTER_TARGET_NAME}

    PUBLIC
        Chicane::Runtime
)

add_dependencies(${SHOOTER_TARGET_NAME} ${SHOOTER_CONTENT_TARGET_NAME})

##################### Setup #######################
CH_ADD_COMPILER(${SHOOTER_TARGET_NAME})

##################### Directories #######################
add_subdirectory(Contents)
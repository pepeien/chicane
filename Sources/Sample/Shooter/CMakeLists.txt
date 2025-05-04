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

if (NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    if (WIN32)
        target_link_options(${SHOOTER_TARGET_NAME} PRIVATE -mwindows)
    endif()
endif()

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
        $<BUILD_INTERFACE:${SHOOTER_BUILD_INTERFACE_DIR}>

        ${INCLUDES_DIR}
)

target_link_libraries(
    ${SHOOTER_TARGET_NAME}

    PRIVATE
        Chicane::Runtime
)

add_dependencies(${SHOOTER_TARGET_NAME} ${SHOOTER_CONTENT_TARGET_NAME})

##################### Setup #######################
CH_ADD_COMPILER(${SHOOTER_TARGET_NAME})

##################### Directories #######################
add_subdirectory(Contents)
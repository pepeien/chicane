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

set_target_properties(
    ${SHOOTER_TARGET_NAME}
    PROPERTIES

    OUTPUT_NAME
    "ShooterSample"

    VS_DEBUGGER_WORKING_DIRECTORY
    "${BUILD_DIR}"

    RUNTIME_OUTPUT_DIRECTORY
    "${BUILD_DIR}"

    RUNTIME_OUTPUT_DIRECTORY_DEBUG
    "${BUILD_DIR}"

    RUNTIME_OUTPUT_DIRECTORY_RELEASE
    "${BUILD_DIR}"
)

target_include_directories(
    ${SHOOTER_TARGET_NAME}

    PUBLIC
        ${CHICANE_INCLUDE_DIRS}
    PRIVATE
        ${SHOOTER_SOURCE_DIR}/Sources
)

find_package(Chicane REQUIRED "C:/Users/Erick/Desktop/projects/game/chicane/engine")

target_link_libraries(
    ${SHOOTER_TARGET_NAME}

    PRIVATE
        Chicane::Runtime
)

if (NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    if (WIN32)
        target_link_options(${SHOOTER_TARGET_NAME} PRIVATE -mwindows)
    endif()
endif()

add_dependencies(${SHOOTER_TARGET_NAME} ${SHOOTER_CONTENT_TARGET_NAME})

##################### Setup #######################
CH_ADD_COMPILER(${SHOOTER_TARGET_NAME})
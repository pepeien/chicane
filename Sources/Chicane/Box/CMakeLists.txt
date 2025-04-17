##################### Files #######################
file(
    GLOB_RECURSE
    HEADERS

    # Sources
    ${BOX_HEADER_DIR}/*.hpp
)

file(
    GLOB_RECURSE
    SOURCES

    # Sources
    ${BOX_SOURCE_DIR}/*.cpp
)

##################### Setup #######################
add_library(${BOX_TARGET_NAME} ${HEADERS} ${SOURCES})

add_library(
    "${NAME}::Box"

    ALIAS
        ${BOX_TARGET_NAME}
)

target_include_directories(
    ${BOX_TARGET_NAME}

    PUBLIC
        ${BOX_INCLUDES}
)

target_link_libraries(
    ${BOX_TARGET_NAME}

    PUBLIC
        ${BOX_LIBRARIES}
)

CH_ADD_COMPILER(${BOX_TARGET_NAME})
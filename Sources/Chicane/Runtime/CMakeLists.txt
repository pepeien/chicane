##################### Vendors #######################
find_package(Vulkan REQUIRED)

##################### Files #######################
file(
    GLOB_RECURSE
    HEADERS

    # Sources
    ${RUNTIME_HEADER_DIR}/*.hpp
)

file(
    GLOB_RECURSE
    SOURCES

    # Sources
    ${RUNTIME_SOURCE_DIR}/*.cpp
)

##################### Setup #######################
add_library(${RUNTIME_TARGET_NAME} ${HEADERS} ${SOURCES})

add_library(
    "${NAME}::Runtime"

    ALIAS
        ${RUNTIME_TARGET_NAME}
)

target_include_directories(
    ${RUNTIME_TARGET_NAME}

    PUBLIC
        ${RUNTIME_INCLUDES}
)

target_link_libraries(
    ${RUNTIME_TARGET_NAME}

    PUBLIC
        ${RUNTIME_LIBRARIES}
)

add_dependencies(
    ${RUNTIME_TARGET_NAME}

    # Contents
    ${RUNTIME_CONTENT_TARGET_NAME}
)

CH_ADD_COMPILER(${RUNTIME_TARGET_NAME})
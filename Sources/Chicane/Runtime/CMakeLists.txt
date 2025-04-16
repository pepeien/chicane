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
        # Alias
        $<BUILD_INTERFACE:${BUILD_INTERFACE_DIR}>
        $<INSTALL_INTERFACE:${INSTALL_INTERFACE_DIR}>

    PRIVATE
        # Systems
        ${RUNTIME_INCLUDE_DIRS}
)

target_link_libraries(
    ${RUNTIME_TARGET_NAME}

    PRIVATE
        # Systems
        ${RUNTIME_LIBRARIES}
)

add_dependencies(
    ${RUNTIME_TARGET_NAME}

    # Contents
    ${RUNTIME_CONTENT_TARGET_NAME}
)

CH_ADD_COMPILER(${RUNTIME_TARGET_NAME})
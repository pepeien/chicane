##################### Files #######################
file(
    GLOB_RECURSE
    HEADERS

    # Sources
    ${CORE_HEADER_DIR}/*.hpp
)

file(
    GLOB_RECURSE
    SOURCES

    # Sources
    ${CORE_SOURCE_DIR}/*.cpp
)

##################### Setup #######################
add_library(${CORE_TARGET_NAME} ${HEADERS} ${SOURCES})

add_library(
    "${NAME}::Core"

    ALIAS
        ${CORE_TARGET_NAME}
)

target_include_directories(
    ${CORE_TARGET_NAME}

    PUBLIC
        # Alias
        $<BUILD_INTERFACE:${BUILD_INTERFACE_DIR}>
        $<INSTALL_INTERFACE:${INSTALL_INTERFACE_DIR}>

    PRIVATE
        # Systems
        ${CORE_INCLUDE_DIRS}
)

target_link_libraries(
    ${CORE_TARGET_NAME}

    PRIVATE
        # Systems
        ${CORE_LIBRARIES}
)

CH_ADD_COMPILER(${CORE_TARGET_NAME})
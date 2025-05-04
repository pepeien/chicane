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

set_target_properties(
    ${RUNTIME_TARGET_NAME}

    PROPERTIES
        OUTPUT_NAME
            "${NAME}Runtime"
        VS_DEBUGGER_WORKING_DIRECTORY
            "${BUILD_DIR}"
)

target_include_directories(
    ${RUNTIME_TARGET_NAME}

    PUBLIC
        # Alias
        $<BUILD_INTERFACE:${BUILD_INTERFACE_DIR}>
        $<INSTALL_INTERFACE:${INSTALL_INTERFACE_DIR}>

        # Vendors
        ${Vulkan_INCLUDE_DIRS}
)

target_link_libraries(
    ${RUNTIME_TARGET_NAME}

    PUBLIC
        # Systems
        Chicane::Box
        Chicane::Core
        Chicane::Grid

        # Vendors
        Vulkan::Vulkan
)

CH_ADD_COMPILER(${RUNTIME_TARGET_NAME})
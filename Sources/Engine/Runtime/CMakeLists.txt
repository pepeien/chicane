##################### Vendors #######################
find_package(Vulkan REQUIRED)

##################### Files #######################
file(
    GLOB_RECURSE
    HEADERS

    # Headers
    ${RUNTIME_HEADER_DIR}/*.hpp
)
list(APPEND HEADERS ${CHICANE_INCLUDES_DIR}/Runtime.hpp)

file(
    GLOB_RECURSE
    SOURCES

    # Sources
    ${RUNTIME_SOURCE_DIR}/*.cpp
)

##################### Setup #######################
if (CHICANE_SHARED)
    add_library(${RUNTIME_TARGET_NAME} SHARED ${HEADERS} ${SOURCES})
else()
    add_library(${RUNTIME_TARGET_NAME} STATIC ${HEADERS} ${SOURCES})
endif()

add_library(
    "${NAME}::Runtime"

    ALIAS
        ${RUNTIME_TARGET_NAME}
)

set_target_properties(
    ${RUNTIME_TARGET_NAME}

    PROPERTIES
        OUTPUT_NAME
            "${NAME}"
        VS_DEBUGGER_WORKING_DIRECTORY
            "${BUILD_DIR}"
        DEFINE_SYMBOL
            "BUILD_EXPORT"
        POSITION_INDEPENDENT_CODE
            TRUE
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
        Chicane::Spring

        # Vendors
        Vulkan::Vulkan

    PRIVATE
        SDL3::SDL3
)

add_dependencies(${RUNTIME_TARGET_NAME} ${CONTENTS_TARGET_NAME})

CH_ADD_COMPILER(${RUNTIME_TARGET_NAME})
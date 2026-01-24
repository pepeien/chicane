##################### Files #######################
file(
    GLOB_RECURSE
    HEADERS

    ${CHICANE_INCLUDES_DIR}/Runtime.hpp
    ${RUNTIME_HEADER_DIR}/*.hpp
)
list(FILTER HEADERS EXCLUDE REGEX "Vendor/.*")

file(
    GLOB_RECURSE
    SOURCES

    ${RUNTIME_SOURCE_DIR}/*.cpp
)
list(FILTER SOURCES EXCLUDE REGEX "Vendor/.*")

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
)

target_link_libraries(
    ${RUNTIME_TARGET_NAME}

    PUBLIC
        Chicane::Box
        Chicane::Core
        Chicane::Grid
        Chicane::Kerb
        Chicane::Renderer
        Chicane::Screech
)

add_dependencies(${RUNTIME_TARGET_NAME} ${CONTENTS_TARGET_NAME})

CH_ADD_COMPILER(${RUNTIME_TARGET_NAME})
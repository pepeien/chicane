##################### Files #######################
file(
    GLOB_RECURSE
    HEADERS
    CONFIGURE_DEPENDS
    ${RUNTIME_HEADER_DIR}/*.hpp
)
list(APPEND HEADERS ${CHICANE_INCLUDES_DIR}/Runtime.hpp)
list(FILTER HEADERS EXCLUDE REGEX "Vendor/.*")

file(
    GLOB_RECURSE
    SOURCES
    CONFIGURE_DEPENDS
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
            "${CHICANE_BUILD_DIR}"
        POSITION_INDEPENDENT_CODE
            TRUE
        CH_REFLECT_HEADERS
            "${HEADERS}"
        CH_BASE_HEADER_DIR
            "${INCLUDES_DIR}"
        CH_SOURCE_HEADER_DIR
            "${RUNTIME_HEADER_DIR}"
)

target_compile_definitions(
    ${RUNTIME_TARGET_NAME}

    PRIVATE
        BUILD_EXPORT
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

CH_ADD_COMPILER(${RUNTIME_TARGET_NAME})
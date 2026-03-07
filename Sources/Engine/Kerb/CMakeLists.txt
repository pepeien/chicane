##################### Files #######################
file(
    GLOB_RECURSE
    HEADERS
    CONFIGURE_DEPENDS
    ${KERB_HEADER_DIR}/*.hpp
)
list(APPEND HEADERS ${CHICANE_INCLUDES_DIR}/Kerb.hpp)
list(FILTER HEADERS EXCLUDE REGEX "Vendor/.*")

file(
    GLOB_RECURSE
    SOURCES
    CONFIGURE_DEPENDS
    ${KERB_SOURCE_DIR}/*.cpp
)
list(APPEND SOURCES ${CHICANE_SOURCES_DIR}/Kerb.cpp)
list(FILTER SOURCES EXCLUDE REGEX "Vendor/.*")

##################### Setup #######################
if (CHICANE_SHARED)
    add_library(${KERB_TARGET_NAME} SHARED ${HEADERS} ${SOURCES})
else()
    add_library(${KERB_TARGET_NAME} STATIC ${HEADERS} ${SOURCES})
endif()

add_library(
    "${NAME}::Kerb"

    ALIAS
        ${KERB_TARGET_NAME}
)

set_target_properties(
    ${KERB_TARGET_NAME}

    PROPERTIES
        OUTPUT_NAME
            "${NAME}Kerb"
        VS_DEBUGGER_WORKING_DIRECTORY
            "${CHICANE_BUILD_DIR}"
        POSITION_INDEPENDENT_CODE
            TRUE
        CH_REFLECT_HEADERS
            "${HEADERS}"
        CH_BASE_HEADER_DIR
            "${INCLUDES_DIR}"
        CH_SOURCE_HEADER_DIR
            "${KERB_HEADER_DIR}"
)

target_compile_definitions(
    ${KERB_TARGET_NAME}

    PRIVATE
        BUILD_EXPORT
)

target_include_directories(
    ${KERB_TARGET_NAME}

    PUBLIC
        # Alias
        $<BUILD_INTERFACE:${BUILD_INTERFACE_DIR}>
        $<INSTALL_INTERFACE:${INSTALL_INTERFACE_DIR}>
)

target_link_libraries(
    ${KERB_TARGET_NAME}

    PUBLIC
        Chicane::Core

        Jolt::Jolt
)

CH_ADD_COMPILER(${KERB_TARGET_NAME})

##################### Directories #######################
add_subdirectory(Vendor)
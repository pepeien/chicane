######################### Files #########################
file(
    GLOB_RECURSE
    HEADERS
    CONFIGURE_DEPENDS
    ${CHICANE_ENGINE_SCREECH_INCLUDES_DIR}/*.hpp
)
list(APPEND HEADERS ${CHICANE_ENGINE_INCLUDES_DIR}/Screech.hpp)
list(FILTER HEADERS EXCLUDE REGEX "Vendor/.*")

file(
    GLOB_RECURSE
    SOURCES
    CONFIGURE_DEPENDS
    ${CHICANE_ENGINE_SCREECH_SOURCES_DIR}/*.cpp
)
list(APPEND SOURCES ${CHICANE_ENGINE_SOURCES_DIR}/Screech.cpp)
list(FILTER SOURCES EXCLUDE REGEX "Vendor/.*")

######################### Setup #########################
if (CHICANE_SHARED)
    add_library(${CHICANE_ENGINE_SCREECH_TARGET_NAME} SHARED ${HEADERS} ${SOURCES})
else()
    add_library(${CHICANE_ENGINE_SCREECH_TARGET_NAME} STATIC ${HEADERS} ${SOURCES})
endif()

add_library(
    "${NAME}::Screech"

    ALIAS
        ${CHICANE_ENGINE_SCREECH_TARGET_NAME}
)

set_target_properties(
    ${CHICANE_ENGINE_SCREECH_TARGET_NAME}

    PROPERTIES
        OUTPUT_NAME
            "${NAME}Screech"
        VS_DEBUGGER_WORKING_DIRECTORY
            "${CHICANE_BUILD_DIR}"
        POSITION_INDEPENDENT_CODE
            TRUE
        CH_REFLECT_HEADERS
            "${HEADERS}"
        CH_BASE_HEADER_DIR
            "${CHICANE_INCLUDES_DIR}"
        CH_SOURCE_HEADER_DIR
            "${CHICANE_ENGINE_SCREECH_INCLUDES_DIR}"
)

target_compile_definitions(
    ${CHICANE_ENGINE_SCREECH_TARGET_NAME}

    PRIVATE
        BUILD_EXPORT
)

target_include_directories(
    ${CHICANE_ENGINE_SCREECH_TARGET_NAME}

    PUBLIC
        $<BUILD_INTERFACE:${CHICANE_BUILD_INTERFACE_DIR}>
        $<INSTALL_INTERFACE:${CHICANE_INSTALL_INTERFACE_DIR}>
)

target_link_libraries(
    ${CHICANE_ENGINE_SCREECH_TARGET_NAME}

    PUBLIC
        Chicane::Box
        Chicane::Core
    PRIVATE
        miniaudio
)

CH_ADD_COMPILER(${CHICANE_ENGINE_SCREECH_TARGET_NAME})

######################### Directories #########################
add_subdirectory(Vendor)
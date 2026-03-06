##################### Files #######################
file(
    GLOB_RECURSE
    HEADERS
    CONFIGURE_DEPENDS
    ${GRID_HEADER_DIR}/*.hpp
)
list(APPEND HEADERS ${CHICANE_INCLUDES_DIR}/Grid.hpp)

file(
    GLOB_RECURSE
    SOURCES
    CONFIGURE_DEPENDS
    ${GRID_SOURCE_DIR}/*.cpp
)
list(APPEND SOURCES ${CHICANE_SOURCES_DIR}/Grid.cpp)

##################### Setup #######################
if (CHICANE_SHARED)
    add_library(${GRID_TARGET_NAME} SHARED ${HEADERS} ${SOURCES})
else()
    add_library(${GRID_TARGET_NAME} STATIC ${HEADERS} ${SOURCES})
endif()

add_library(
    "${NAME}::Grid"

    ALIAS
        ${GRID_TARGET_NAME}
)

set_target_properties(
    ${GRID_TARGET_NAME}

    PROPERTIES
        OUTPUT_NAME
            "${NAME}Grid"
        VS_DEBUGGER_WORKING_DIRECTORY
            "${CHICANE_BUILD_DIR}"
        POSITION_INDEPENDENT_CODE
            TRUE
        CH_REFLECT_HEADERS
            "${HEADERS}"
        CH_BASE_HEADER_DIR
            "${INCLUDES_DIR}"
        CH_SOURCE_HEADER_DIR
            "${GRID_HEADER_DIR}"
)

target_compile_definitions(
    ${GRID_TARGET_NAME}

    PRIVATE
        BUILD_EXPORT
)

target_include_directories(
    ${GRID_TARGET_NAME}

    PUBLIC
        # Alias
        $<BUILD_INTERFACE:${BUILD_INTERFACE_DIR}>
        $<INSTALL_INTERFACE:${INSTALL_INTERFACE_DIR}>
)

target_link_libraries(
    ${GRID_TARGET_NAME}

    PUBLIC
        Chicane::Box
        Chicane::Core
)

CH_ADD_COMPILER(${GRID_TARGET_NAME})
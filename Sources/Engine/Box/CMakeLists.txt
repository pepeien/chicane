##################### Files #######################
file(
    GLOB_RECURSE
    HEADERS

    # Headers
    ${BOX_HEADER_DIR}/*.hpp
)
list(APPEND HEADERS ${CHICANE_INCLUDES_DIR}/Box.hpp)
list(FILTER HEADERS EXCLUDE REGEX "Vendors/.*")

file(
    GLOB_RECURSE
    SOURCES

    # Sources
    ${BOX_SOURCE_DIR}/*.cpp
)
list(APPEND SOURCES ${CHICANE_SOURCES_DIR}/Box.cpp)
list(FILTER SOURCES EXCLUDE REGEX "Vendors/.*")

##################### Setup #######################
if (CHICANE_SHARED)
    add_library(${BOX_TARGET_NAME} SHARED ${HEADERS} ${SOURCES})
else()
    add_library(${BOX_TARGET_NAME} STATIC ${HEADERS} ${SOURCES})
endif()

add_library(
    "${NAME}::Box"

    ALIAS
        ${BOX_TARGET_NAME}
)

set_target_properties(
    ${BOX_TARGET_NAME}

    PROPERTIES
        OUTPUT_NAME
            "${NAME}Box"
        VS_DEBUGGER_WORKING_DIRECTORY
            "${BUILD_DIR}"
        DEFINE_SYMBOL
            "BUILD_EXPORT"
        POSITION_INDEPENDENT_CODE
            TRUE
)

target_include_directories(
    ${BOX_TARGET_NAME}

    PUBLIC
        # Alias
        $<BUILD_INTERFACE:${BUILD_INTERFACE_DIR}>
        $<INSTALL_INTERFACE:${INSTALL_INTERFACE_DIR}>
)

target_link_libraries(
    ${BOX_TARGET_NAME}

    PUBLIC
        # Systems
        Chicane::Core

    PRIVATE
        # Vendors
        fast_obj
        freetype
)

CH_ADD_COMPILER(${BOX_TARGET_NAME})

##################### Directories #######################
add_subdirectory(Vendor)
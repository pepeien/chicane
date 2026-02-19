##################### Values #######################

# Dirs
set(VENDORS_DIR "${CMAKE_CURRENT_LIST_DIR}/Vendor")

##################### Files #######################
file(
    GLOB_RECURSE
    HEADERS

    # Headers
    ${CORE_HEADER_DIR}/*.hpp
)
list(APPEND HEADERS ${CHICANE_INCLUDES_DIR}/Core.hpp)

file(
    GLOB_RECURSE
    SOURCES

    # Sources
    ${CORE_SOURCE_DIR}/*.cpp
)
list(FILTER SOURCES EXCLUDE REGEX "Vendor/.*")

##################### Setup #######################
if (CHICANE_SHARED)
    add_library(${CORE_TARGET_NAME} SHARED ${HEADERS} ${SOURCES})
else()
    add_library(${CORE_TARGET_NAME} STATIC ${HEADERS} ${SOURCES})
endif()

add_library(
    "${NAME}::Core"

    ALIAS
        ${CORE_TARGET_NAME}
)

set_target_properties(
    ${CORE_TARGET_NAME}

    PROPERTIES
        OUTPUT_NAME
            "${NAME}Core"
        VS_DEBUGGER_WORKING_DIRECTORY
            "${BUILD_DIR}"
        DEFINE_SYMBOL
            "BUILD_EXPORT"
        POSITION_INDEPENDENT_CODE
            TRUE
)

target_include_directories(
    ${CORE_TARGET_NAME}

    PUBLIC
        # Alias
        $<BUILD_INTERFACE:${BUILD_INTERFACE_DIR}>
        $<INSTALL_INTERFACE:${INSTALL_INTERFACE_DIR}>

    PRIVATE
        # Vendors
        ${VENDORS_DIR}/stb
        ${VENDORS_DIR}/xxHash
)

target_link_libraries(
    ${CORE_TARGET_NAME}
 
    PUBLIC
        glm::glm
        pugixml::pugixml
        SDL3::SDL3

    PRIVATE
        base64
        libtess2::libtess2
)

CH_ADD_COMPILER(${CORE_TARGET_NAME})

##################### Directories #######################
add_subdirectory(Vendor)
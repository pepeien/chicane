######################### Values #########################
## Dirs
set(VENDORS_DIR "${CMAKE_CURRENT_LIST_DIR}/Vendor")

######################### Files #########################
file(
    GLOB_RECURSE
    HEADERS
    CONFIGURE_DEPENDS
    ${CHICANE_ENGINE_CORE_INCLUDES_DIR}/*.hpp
)
list(APPEND HEADERS ${CHICANE_ENGINE_INCLUDES_DIR}/Core.hpp)

file(
    GLOB_RECURSE
    SOURCES
    CONFIGURE_DEPENDS
    ${CHICANE_ENGINE_CORE_SOURCES_DIR}/*.cpp
)
list(FILTER SOURCES EXCLUDE REGEX "Vendor/.*")

######################### Setup #########################
if (CHICANE_SHARED)
    add_library(${CHICANE_ENGINE_CORE_TARGET_NAME} SHARED ${HEADERS} ${SOURCES})
else()
    add_library(${CHICANE_ENGINE_CORE_TARGET_NAME} STATIC ${HEADERS} ${SOURCES})
endif()

add_library(
    "${NAME}::Core"

    ALIAS
        ${CHICANE_ENGINE_CORE_TARGET_NAME}
)

set_target_properties(
    ${CHICANE_ENGINE_CORE_TARGET_NAME}

    PROPERTIES
        OUTPUT_NAME
            "${NAME}Core"
        VS_DEBUGGER_WORKING_DIRECTORY
            "${CHICANE_BUILD_DIR}"
        POSITION_INDEPENDENT_CODE
            TRUE
        CH_REFLECT_HEADERS
            "${HEADERS}"
        CH_BASE_HEADER_DIR
            "${CHICANE_INCLUDES_DIR}"
        CH_SOURCE_HEADER_DIR
            "${CHICANE_ENGINE_CORE_INCLUDES_DIR}"
)

target_compile_definitions(
    ${CHICANE_ENGINE_CORE_TARGET_NAME}

    PRIVATE
        BUILD_EXPORT
)

target_include_directories(
    ${CHICANE_ENGINE_CORE_TARGET_NAME}

    PUBLIC
        $<BUILD_INTERFACE:${CHICANE_BUILD_INTERFACE_DIR}>
        $<INSTALL_INTERFACE:${CHICANE_INSTALL_INTERFACE_DIR}>

    PRIVATE
        ${VENDORS_DIR}/stb
        ${VENDORS_DIR}/xxHash
)

target_link_libraries(
    ${CHICANE_ENGINE_CORE_TARGET_NAME}
 
    PUBLIC
        glm::glm
        pugixml::pugixml
        SDL3::SDL3

    PRIVATE
        base64
        libtess2::libtess2
)

CH_ADD_COMPILER(${CHICANE_ENGINE_CORE_TARGET_NAME})

######################### Directories #########################
add_subdirectory(Vendor)
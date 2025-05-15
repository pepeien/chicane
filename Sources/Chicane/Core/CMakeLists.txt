##################### Files #######################
file(
    GLOB_RECURSE
    HEADERS

    # Headers
    ${CORE_HEADER_DIR}/*.hpp
)

file(
    GLOB_RECURSE
    SOURCES

    # Sources
    ${CORE_SOURCE_DIR}/*.cpp
)

##################### Setup #######################
add_library(${CORE_TARGET_NAME} ${HEADERS} ${SOURCES})

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

    PUBLIC
        # Vendors
        ${VENDORS_DIR}/stb
)

target_link_libraries(
    ${CORE_TARGET_NAME}
 
    PUBLIC
        # Vendors
        glm::glm
        pugixml::pugixml
        SDL3::SDL3
)

CH_ADD_COMPILER(${CORE_TARGET_NAME})
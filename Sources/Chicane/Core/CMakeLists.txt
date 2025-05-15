##################### Files #######################
file(
    GLOB_RECURSE
    SOURCES

    # Sources
    ${CORE_SOURCE_DIR}/*.cpp
)

##################### Setup #######################
add_library(${CORE_TARGET_NAME} ${SOURCES})

add_library(
    "${NAME}::Core"

    ALIAS
        ${CORE_TARGET_NAME}
)

if (BUILD_SHARED_LIBS)
    target_compile_definitions(${CORE_TARGET_NAME} PRIVATE BUILD_EXPORT)
endif()

set_target_properties(
    ${CORE_TARGET_NAME}

    PROPERTIES
        OUTPUT_NAME
            "${NAME}Core"
        VS_DEBUGGER_WORKING_DIRECTORY
            "${BUILD_DIR}"
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
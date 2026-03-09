######################### Files ##########################
file(
    GLOB_RECURSE
    HEADERS
    CONFIGURE_DEPENDS
    ${CHICANE_EDITOR_INCLUDES_DIR}/*.hpp
)

file(
    GLOB_RECURSE
    SOURCES
    CONFIGURE_DEPENDS
    ${CHICANE_EDITOR_SOURCES_DIR}/*.cpp
)

##################### Setup #######################
if (NOT CHICANE_VULKAN)
    list(FILTER HEADERS EXCLUDE REGEX "${CMAKE_CURRENT_LIST_DIR}/Layer/Vulkan")
    list(FILTER SOURCES EXCLUDE REGEX "${CMAKE_CURRENT_LIST_DIR}/Layer/Vulkan")
endif()

if (NOT CHICANE_OPENGL)
    list(FILTER HEADERS EXCLUDE REGEX "${CMAKE_CURRENT_LIST_DIR}/Layer/OpenGL")
    list(FILTER SOURCES EXCLUDE REGEX "${CMAKE_CURRENT_LIST_DIR}/Layer/OpenGL")
endif()

##################### Directories #######################
add_executable(${CHICANE_EDITOR_TARGET_NAME} ${HEADERS} ${SOURCES})

set_target_properties(
    ${CHICANE_EDITOR_TARGET_NAME}

    PROPERTIES
        OUTPUT_NAME
            "${NAME}Editor"
        VS_DEBUGGER_WORKING_DIRECTORY
            "${CHICANE_BUILD_DIR}"
        CH_REFLECT_HEADERS
            "${HEADERS}"
        CH_BASE_HEADER_DIR
            "${CHICANE_SOURCES_DIR}"
        CH_SOURCE_HEADER_DIR
            "${CHICANE_EDITOR_SOURCES_DIR}"
)

target_include_directories(
    ${CHICANE_EDITOR_TARGET_NAME}

    PUBLIC
        $<BUILD_INTERFACE:${CHICANE_BUILD_INTERFACE_DIR}>
        $<INSTALL_INTERFACE:${CHICANE_INSTALL_INTERFACE_DIR}>
    PRIVATE
        ${CHICANE_SOURCES_DIR}
)

target_link_libraries(
    ${CHICANE_EDITOR_TARGET_NAME}

    PRIVATE
        Chicane::Runtime
)

CH_ADD_COMPILER(${CHICANE_EDITOR_TARGET_NAME})

######################### Depedencies #########################
add_dependencies(${CHICANE_EDITOR_TARGET_NAME} ${CHICANE_EDITOR_ASSETS_TARGET_NAME})
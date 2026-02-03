######################### Files ##########################
file(
    GLOB_RECURSE
    HEADERS

    ${CMAKE_CURRENT_LIST_DIR}/*.hpp
)

file(
    GLOB_RECURSE
    SOURCES

    ${CMAKE_CURRENT_LIST_DIR}/*.cpp
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
add_executable(${EDITOR_TARGET_NAME} ${HEADERS} ${SOURCES})

set_target_properties(
    ${EDITOR_TARGET_NAME}

    PROPERTIES
        OUTPUT_NAME
            "ChicaneEditor"
        VS_DEBUGGER_WORKING_DIRECTORY
            "${BUILD_DIR}"
)

target_include_directories(
    ${EDITOR_TARGET_NAME}

    PUBLIC
        $<BUILD_INTERFACE:${BUILD_INTERFACE_DIR}>
        $<INSTALL_INTERFACE:${INSTALL_INTERFACE_DIR}>
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(
    ${EDITOR_TARGET_NAME}

    PRIVATE
        Chicane::Runtime
)

add_dependencies(${EDITOR_TARGET_NAME} ${EDITOR_CONTENT_TARGET_NAME})

CH_ADD_COMPILER(${EDITOR_TARGET_NAME})
##################### Values #######################

# Dirs
set(VENDORS_DIR "${CMAKE_CURRENT_LIST_DIR}/Vendor")

##################### Files #######################
file(
    GLOB_RECURSE
    HEADERS

    ${CHICANE_INCLUDES_DIR}/Renderer.hpp
    ${RENDERER_HEADER_DIR}/*.hpp
)
list(FILTER HEADERS EXCLUDE REGEX "Vendor/.*")

file(
    GLOB_RECURSE
    SOURCES

    ${RENDERER_SOURCE_DIR}/*.cpp
)
list(FILTER SOURCES EXCLUDE REGEX "Vendor/.*")

##################### Setup #######################
if (CHICANE_OPENGL)
    find_package(OpenGL REQUIRED)
    list(APPEND PRIVATE_LIBS OpenGL::GL)

    add_subdirectory(${VENDORS_DIR}/Glad)
    list(APPEND PRIVATE_LIBS Glad::Glad)
else()
    list(FILTER HEADERS EXCLUDE REGEX "${RENDERER_HEADER_DIR}/Backend/OpenGL")
    list(FILTER SOURCES EXCLUDE REGEX "${RENDERER_SOURCE_DIR}/Backend/OpenGL")
endif()

if (CHICANE_VULKAN)
    find_package(Vulkan REQUIRED)
    list(APPEND PUBLIC_LIBS Vulkan::Vulkan)
else()
    list(FILTER HEADERS EXCLUDE REGEX "${RENDERER_HEADER_DIR}/Backend/Vulkan")
    list(FILTER SOURCES EXCLUDE REGEX "${RENDERER_SOURCE_DIR}/Backend/Vulkan")
endif()


if (CHICANE_SHARED)
    add_library(${RENDERER_TARGET_NAME} SHARED ${HEADERS} ${SOURCES})
else()
    add_library(${RENDERER_TARGET_NAME} STATIC ${HEADERS} ${SOURCES})
endif()

add_library(
    "${NAME}::Renderer"

    ALIAS
        ${RENDERER_TARGET_NAME}
)

set_target_properties(
    ${RENDERER_TARGET_NAME}

    PROPERTIES
        OUTPUT_NAME
            "${NAME}Renderer"
        VS_DEBUGGER_WORKING_DIRECTORY
            "${CHICANE_BUILD_DIR}"
        POSITION_INDEPENDENT_CODE
            TRUE
)

target_compile_definitions(
    ${RENDERER_TARGET_NAME}

    PRIVATE
        BUILD_EXPORT
)

target_include_directories(
    ${RENDERER_TARGET_NAME}

    PUBLIC
        # Alias
        $<BUILD_INTERFACE:${BUILD_INTERFACE_DIR}>
        $<INSTALL_INTERFACE:${INSTALL_INTERFACE_DIR}>
)

target_link_libraries(
    ${RENDERER_TARGET_NAME}

    PUBLIC
        Chicane::Core

        ${PUBLIC_LIBS}
    PRIVATE
        ${PRIVATE_LIBS}
)

CH_ADD_COMPILER(${RENDERER_TARGET_NAME})
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
if (CHICANE_VULKAN)
    find_package(Vulkan REQUIRED)

    list(APPEND GRAPHICS_LIBS Vulkan::Vulkan)
else()
    list(FILTER HEADERS EXCLUDE REGEX "Renderer/Vulkan")
    list(FILTER SOURCES EXCLUDE REGEX "Renderer/Vulkan")
endif()

if (CHICANE_OPENGL)
    find_package(OpenGL REQUIRED)
    find_package(GLEW REQUIRED)

    list(APPEND GRAPHICS_LIBS OpenGL::GL)
    list(APPEND GRAPHICS_LIBS GLEW)
else()
    list(FILTER HEADERS EXCLUDE REGEX "Renderer/OpenGL")
    list(FILTER SOURCES EXCLUDE REGEX "Renderer/OpenGL")
endif()

if (CHICANE_SHARED)
    add_library(${RENDERER_TARGET_NAME} SHARED ${HEADERS} ${SOURCES})
else()
    add_library(${RENDERER_TARGET_NAME} STATIC ${HEADERS} ${SOURCES})
endif()

if (CHICANE_VULKAN)
    target_compile_definitions(
        ${RENDERER_TARGET_NAME}

        PRIVATE
            CHICANE_VULKAN
    )
endif()

if (CHICANE_OPENGL)
    target_compile_definitions(
        ${RENDERER_TARGET_NAME}

        PRIVATE
            CHICANE_OPENGL
    )
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
            "${BUILD_DIR}"
        DEFINE_SYMBOL
            "BUILD_EXPORT"
        POSITION_INDEPENDENT_CODE
            TRUE
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
        Chicane::Box
        Chicane::Core

        ${GRAPHICS_LIBS}
)

CH_ADD_COMPILER(${RENDERER_TARGET_NAME})
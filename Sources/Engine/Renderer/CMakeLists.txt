######################### Directories #########################
add_subdirectory(Vendor)

######################### Files #########################
file(
    GLOB_RECURSE
    HEADERS
    CONFIGURE_DEPENDS
    ${CHICANE_ENGINE_RENDERER_INCLUDES_DIR}/*.hpp
)
list(APPEND HEADERS ${CHICANE_ENGINE_INCLUDES_DIR}/Renderer.hpp)
list(FILTER HEADERS EXCLUDE REGEX "Vendor/.*")

file(
    GLOB_RECURSE
    SOURCES
    CONFIGURE_DEPENDS
    ${CHICANE_ENGINE_RENDERER_SOURCES_DIR}/*.cpp
)
list(FILTER SOURCES EXCLUDE REGEX "Vendor/.*")

file(
    GLOB_RECURSE
    PRIVATE_HEADERS
    CONFIGURE_DEPENDS
    ${CHICANE_ENGINE_RENDERER_SOURCES_DIR}/Backend/*.hpp
)
list(FILTER PRIVATE_HEADERS EXCLUDE REGEX "Vendor/.*")

######################### Setup #########################
if (CHICANE_OPENGL)
    find_package(OpenGL REQUIRED)
    list(APPEND BACKEND_LIBS OpenGL::GL)
    list(APPEND BACKEND_LIBS Glad::Glad)
else()
    list(FILTER PRIVATE_HEADERS EXCLUDE REGEX "${CHICANE_ENGINE_RENDERER_SOURCES_DIR}/Backend/OpenGL")
    list(FILTER SOURCES EXCLUDE REGEX "${CHICANE_ENGINE_RENDERER_SOURCES_DIR}/Backend/OpenGL")
endif()

if (CHICANE_VULKAN)
    find_package(Vulkan REQUIRED)
    list(APPEND BACKEND_LIBS Vulkan::Vulkan)
    list(APPEND PRIVATE_LIBS GPUOpen::VulkanMemoryAllocator)
else()
    list(FILTER PRIVATE_HEADERS EXCLUDE REGEX "${CHICANE_ENGINE_RENDERER_SOURCES_DIR}/Backend/Vulkan")
    list(FILTER SOURCES EXCLUDE REGEX "${CHICANE_ENGINE_RENDERER_SOURCES_DIR}/Backend/Vulkan")
endif()

# Shared Renderer links loaders inside the DLL. Static still has to expose them
# so the final binary can resolve Vulkan/OpenGL/Glad symbols.
if (CHICANE_SHARED)
    list(APPEND PRIVATE_LIBS ${BACKEND_LIBS})
else()
    list(APPEND PUBLIC_LIBS ${BACKEND_LIBS})
endif()


if (CHICANE_SHARED)
    add_library(${CHICANE_ENGINE_RENDERER_TARGET_NAME} SHARED ${HEADERS} ${PRIVATE_HEADERS} ${SOURCES})
else()
    add_library(${CHICANE_ENGINE_RENDERER_TARGET_NAME} STATIC ${HEADERS} ${PRIVATE_HEADERS} ${SOURCES})
endif()

add_library(
    "${NAME}::Renderer"

    ALIAS
        ${CHICANE_ENGINE_RENDERER_TARGET_NAME}
)

set_target_properties(
    ${CHICANE_ENGINE_RENDERER_TARGET_NAME}

    PROPERTIES
        OUTPUT_NAME
            "${NAME}Renderer"
        VS_DEBUGGER_WORKING_DIRECTORY
            "${CHICANE_BUILD_DIR}"
        POSITION_INDEPENDENT_CODE
            TRUE
        CH_REFLECT_HEADERS
            "${HEADERS}"
        CH_BASE_HEADER_DIR
            "${CHICANE_INCLUDES_DIR}"
        CH_SOURCE_HEADER_DIR
            "${CHICANE_ENGINE_RENDERER_INCLUDES_DIR}"
)

target_compile_definitions(
    ${CHICANE_ENGINE_RENDERER_TARGET_NAME}

    PRIVATE
        BUILD_EXPORT
)

target_include_directories(
    ${CHICANE_ENGINE_RENDERER_TARGET_NAME}

    PUBLIC
        $<BUILD_INTERFACE:${CHICANE_BUILD_INTERFACE_DIR}>
        $<INSTALL_INTERFACE:${CHICANE_INSTALL_INTERFACE_DIR}>
    PRIVATE
        ${CHICANE_ENGINE_RENDERER_SOURCES_DIR}
)

target_link_libraries(
    ${CHICANE_ENGINE_RENDERER_TARGET_NAME}

    PUBLIC
        Chicane::Core

        ${PUBLIC_LIBS}
    PRIVATE
        ${PRIVATE_LIBS}
)

CH_ADD_COMPILER(${CHICANE_ENGINE_RENDERER_TARGET_NAME})
CH_GENERATE_REFLECTION(${CHICANE_ENGINE_RENDERER_TARGET_NAME})

######################### Depedencies #########################
add_dependencies(${CHICANE_ENGINE_RENDERER_TARGET_NAME} ${CHICANE_ENGINE_ASSETS_TARGET_NAME})
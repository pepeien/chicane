##################### Files #######################
file(
    GLOB_RECURSE
    HEADERS

    ${CHICANE_INCLUDES_DIR}/Runtime.hpp
    ${RUNTIME_HEADER_DIR}/*.hpp
)
list(FILTER HEADERS EXCLUDE REGEX "Vendor/.*")

file(
    GLOB_RECURSE
    SOURCES

    ${RUNTIME_SOURCE_DIR}/*.cpp
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

    list(APPEND GRAPHICS_LIBS OpenGL::GL)
else()
    list(FILTER HEADERS EXCLUDE REGEX "Renderer/OpenGL")
    list(FILTER SOURCES EXCLUDE REGEX "Renderer/OpenGL")
endif()

if (CHICANE_SHARED)
    add_library(${RUNTIME_TARGET_NAME} SHARED ${HEADERS} ${SOURCES})
else()
    add_library(${RUNTIME_TARGET_NAME} STATIC ${HEADERS} ${SOURCES})
endif()

if (CHICANE_VULKAN)
    target_compile_definitions(
        ${RUNTIME_TARGET_NAME}

        PRIVATE
            CHICANE_VULKAN
    )
endif()

add_library(
    "${NAME}::Runtime"

    ALIAS
        ${RUNTIME_TARGET_NAME}
)

set_target_properties(
    ${RUNTIME_TARGET_NAME}

    PROPERTIES
        OUTPUT_NAME
            "${NAME}"
        VS_DEBUGGER_WORKING_DIRECTORY
            "${BUILD_DIR}"
        DEFINE_SYMBOL
            "BUILD_EXPORT"
        POSITION_INDEPENDENT_CODE
            TRUE
)

target_include_directories(
    ${RUNTIME_TARGET_NAME}

    PUBLIC
        # Alias
        $<BUILD_INTERFACE:${BUILD_INTERFACE_DIR}>
        $<INSTALL_INTERFACE:${INSTALL_INTERFACE_DIR}>
)

target_link_libraries(
    ${RUNTIME_TARGET_NAME}

    PUBLIC
        Chicane::Box
        Chicane::Core
        Chicane::Grid
        Chicane::Kerb
        Chicane::Screech

        ${GRAPHICS_LIBS}

    PRIVATE
        SDL3::SDL3
)

add_dependencies(${RUNTIME_TARGET_NAME} ${CONTENTS_TARGET_NAME})

CH_ADD_COMPILER(${RUNTIME_TARGET_NAME})

##################### Directories #######################
add_subdirectory(Vendor)
##################### Files #######################
file(
    GLOB_RECURSE
    HEADERS

    # Headers
    ${CORE_HEADER_DIR}/*.hpp
)
list(FILTER HEADERS EXCLUDE REGEX "Vendors/.*")

file(
    GLOB_RECURSE
    SOURCES

    # Sources
    ${CORE_SOURCE_DIR}/*.cpp
)
list(FILTER SOURCES EXCLUDE REGEX "Vendors/.*")

##################### Values #######################
# Base64
set(BASE64_ENABLE_TESTING OFF CACHE BOOL "")

# Earcut
set(EARCUT_BUILD_TESTS OFF CACHE BOOL "")
set(EARCUT_BUILD_BENCH OFF CACHE BOOL "")
set(EARCUT_BUILD_VIZ   OFF CACHE BOOL "")

# SDL3
set(SDL_AUDIO ON CACHE BOOL "")

set(SDL_VIDEO ON CACHE BOOL "")

set(SDL_GPU     ON CACHE BOOL "")
set(SDL_OPENGL  ON CACHE BOOL "")
set(SDL_VULKAN  ON CACHE BOOL "")
set(SDL_DIRECTX OFF CACHE BOOL "")

set(SDL_RENDER OFF CACHE BOOL "")

set(SDL_CAMERA OFF CACHE BOOL "")

set(SDL_JOYSTICK ON CACHE BOOL "")
set(SDL_HIDAPI   ON CACHE BOOL "")
set(SDL_HAPTIC   OFF CACHE BOOL "")
set(SDL_SENSOR   OFF CACHE BOOL "")

set(SDL_POWER OFF CACHE BOOL "")

set(SDL_DIALOG OFF CACHE BOOL "")

set(SDL_EXAMPLES OFFCACHE BOOL "")

set(SDL_EXAMPLES OFF CACHE BOOL "")

set(SDL_TEST_LIBRARY OFF CACHE BOOL "")

set(SDL_PLATFORM_GDK OFF CACHE BOOL "")

# glm
set(GLM_TESTS OFF CACHE BOOL "")

# Dirs
set(VENDORS_DIR  "${CMAKE_CURRENT_LIST_DIR}/Vendors")

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
        base64
        earcut_hpp::earcut_hpp
        glm::glm
        pugixml::pugixml
        SDL3::SDL3
)

CH_ADD_COMPILER(${CORE_TARGET_NAME})

##################### Directories #######################
add_subdirectory(Vendors)
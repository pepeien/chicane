##############  Set values  #######################
set(TARGET_NAME "${ENGINE_TARGET_NAME}")
set(SOURCES_DIR "${ENGINE_SOURCE_DIR}")

# Shader
set(ENGINE_SHADERS_TARGET_NAME  "Shaders")

set(ENGINE_SHADERS_SOURCE_DIR "${ENGINE_DIR}/Shaders")
set(ENGINE_SHADER_BUILD_DIR   "${ENGINE_BUILD_DIR}/Shaders")

##############  Set source files  #######################
file(
    GLOB_RECURSE
    SOURCES
 
    ${SOURCES_DIR}/*.cpp
)

add_library(${TARGET_NAME} STATIC ${SOURCES})

set_target_properties(${TARGET_NAME} PROPERTIES PREFIX "")

############## Set Property Balues  #######################
set_property(
    TARGET
    ${TARGET_NAME}
    PROPERTY
    VS_DEBUGGER_WORKING_DIRECTORY
    "${ENGINE_BUILD_DIR}"
)

set_property(
    TARGET
    ${TARGET_NAME}
    PROPERTY
    ARCHIVE_OUTPUT_DIRECTORY
    "${ENGINE_BUILD_DIR}"
)

set_property(
    TARGET
    ${TARGET_NAME}
    PROPERTY
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG
    "${ENGINE_BUILD_DIR}"
)

set_property(
    TARGET
    ${TARGET_NAME}
    PROPERTY
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE
    "${ENGINE_BUILD_DIR}"
)

############## Linking Libs  #######################

find_package(Vulkan REQUIRED)

if (NOT Vulkan_FOUND)
    message(FATAL_ERROR "Vulkan not found")
endif()

target_include_directories(
    ${TARGET_NAME}
    PUBLIC

    # Source
    ${SOURCES_DIR}

    # Vendor
    ${ENGINE_INCLUDE_STB_PATH}
)

if (WIN32)
    message(STATUS "CREATING BUILD FOR WINDOWS")

    if (USE_MINGW)
        target_include_directories(
            ${TARGET_NAME}
            PUBLIC

            ${MINGW_PATH}/include
        )

        target_link_directories(
            ${TARGET_NAME}
            PUBLIC

            ${MINGW_PATH}/lib
        )
    endif()

    target_include_directories(
        ${TARGET_NAME}
        PUBLIC

        # Vendor
        ${Vulkan_INCLUDE_DIRS}
    )

    target_link_directories(
        ${TARGET_NAME}
        PUBLIC

        # Vulkan
        $ENV{VULKAN_SDK}/Lib
    )

    target_link_libraries(
        ${TARGET_NAME}
        PUBLIC

        # Vendor
        vulkan-1
        SDL2
    )
elseif(UNIX)
    message(STATUS "CREATING BUILD FOR UNIX")

    find_package(glm  REQUIRED)
    find_package(SDL2 REQUIRED)

    target_link_libraries(
        ${TARGET_NAME}
        PUBLIC

        # Vendor
        vulkan
        glm::glm
        SDL2::SDL2
    )
endif()

add_subdirectory(Shaders)
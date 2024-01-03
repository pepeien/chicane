##############  Set values  #######################
set(TARGET_NAME "${ENGINE_TARGET_NAME}")
set(SOURCES_DIR "${ENGINE_SOURCE_DIR}")

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
endif()

target_include_directories(
    ${TARGET_NAME}
    PUBLIC

    # Source
    ${SOURCES_DIR}

    # Vendor
    ${ENGINE_INCLUDE_STB_PATH}
    ${ENGINE_INCLUDE_VULKAN_PATH}
)

target_link_directories(
    ${TARGET_NAME}
    PUBLIC

    # Vendor
    ${ENGINE_LIBRARY_VULKAN_PATH}
)

target_link_libraries(
    ${TARGET_NAME}
    PRIVATE

    # Vendor
    vulkan-1
    SDL2
)

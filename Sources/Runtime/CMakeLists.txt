##############  Set source files  #######################
file(
    GLOB_RECURSE
    SOURCES
 
    ${RUNTIME_RUNTIME_DIR}/*.cpp
)

add_library(${RUNTIME_TARGET_NAME} STATIC ${SOURCES})

############## Set Property Balues  #######################
set_property(
    TARGET
    ${RUNTIME_TARGET_NAME}
    PROPERTY
    VS_DEBUGGER_WORKING_DIRECTORY
    "${RUNTIME_BUILD_DIR}"
)

set_property(
    TARGET
    ${RUNTIME_TARGET_NAME}
    PROPERTY
    ARCHIVE_OUTPUT_DIRECTORY
    "${RUNTIME_BUILD_DIR}"
)

set_property(
    TARGET
    ${RUNTIME_TARGET_NAME}
    PROPERTY
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG
    "${RUNTIME_BUILD_DIR}"
)

set_property(
    TARGET
    ${RUNTIME_TARGET_NAME}
    PROPERTY
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE
    "{RUNTIME_BUILD_DIR}"
)

############## Linking Libs  #######################

find_package(Vulkan REQUIRED)

if (NOT Vulkan_FOUND)
    message(FATAL_ERROR "Vulkan not found")
endif()

target_include_directories(
    ${RUNTIME_TARGET_NAME}
    PUBLIC

    # Source
    ${RUNTIME_RUNTIME_DIR}

    # Vendor
    ${VENDOR_DIR}
)
message("${VENDOR_DIR}")
if (WIN32)
    message(STATUS "CREATING BUILD FOR WINDOWS")

    if (USE_MINGW)
        target_include_directories(
            ${RUNTIME_TARGET_NAME}
            PUBLIC

            ${MINGW_PATH}/include
        )

        target_link_directories(
            ${RUNTIME_TARGET_NAME}
            PUBLIC

            ${MINGW_PATH}/lib
        )
    endif()

    target_include_directories(
        ${RUNTIME_TARGET_NAME}
        PUBLIC

        # Vendor
        ${Vulkan_INCLUDE_DIRS}
    )

    target_link_directories(
        ${RUNTIME_TARGET_NAME}
        PUBLIC

        # Vulkan
        $ENV{VULKAN_SDK}/Lib
    )

    target_link_libraries(
        ${RUNTIME_TARGET_NAME}
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
        ${RUNTIME_TARGET_NAME}
        PUBLIC

        # Vendor
        vulkan
        glm::glm
        SDL2::SDL2
    )
endif()
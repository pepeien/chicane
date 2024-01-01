##############  Set values  #######################
set(TARGET_NAME "${RUNTIME_TARGET_NAME}")
set(SOURCES_DIR "${RUNTIME_SOURCE_DIR}")

##############  Set source files  #######################
file(
  GLOB_RECURSE
  SOURCES
  
  ${SOURCES_DIR}/*.cpp
)

add_library(${TARGET_NAME} ${SOURCES})

############## Set Property Balues  #######################
set_property(
    TARGET
    ${TARGET_NAME}
    PROPERTY
    VS_DEBUGGER_WORKING_DIRECTORY
    "${BUILD_BINARY_DIR}"
)

############## Linking Libs  #######################
if (WIN32)
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

if (DEFINED VULKAN_PATH)
    set(Vulkan_INCLUDE_DIRS "${VULKAN_PATH}/Include")
    set(Vulkan_LIBRARIES "${VULKAN_PATH}/Lib")
    set(Vulkan_FOUND "True")
else()
    find_package(Vulkan REQUIRED)
  
    message(STATUS "Found Vulkan: $ENV{VULKAN_SDK}")
endif()
if (NOT Vulkan_FOUND)
    message(FATAL_ERROR "Could not find Vulkan library!")
else()
    message(STATUS "Using vulkan lib at: ${Vulkan_LIBRARIES}")
endif()

target_include_directories(
    ${TARGET_NAME}
    PRIVATE

    # Source
    ${SOURCES_DIR}
)

set(SDL2_DIR "${VULKAN_PATH}/cmake")

find_package(SDL2 REQUIRED)

target_include_directories(
    ${TARGET_NAME}
    PUBLIC

    # Source
    ${SOURCES_DIR}

    # Third Party
    ${GLM_PATH}
    ${STB_PATH}
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

        # Third Party
        ${Vulkan_INCLUDE_DIRS}
    )

    target_link_directories(
        ${TARGET_NAME}
        PUBLIC

        ${Vulkan_LIBRARIES}
    )

    target_link_libraries(
        ${TARGET_NAME}
        PUBLIC

        vulkan-1
        SDL2::SDL2
    )
elseif (UNIX)
    message(STATUS "CREATING BUILD FOR UNIX")

    target_link_libraries(
        ${TARGET_NAME}
        PUBLIC

        ${Vulkan_LIBRARIES}
    )
endif()
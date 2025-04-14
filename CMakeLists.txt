##################### Includes #######################
include(./.env.cmake OPTIONAL RESULT_VARIABLE LOCAL_ENV)

##################### Settings #######################
cmake_minimum_required(VERSION 3.11.0)

set(CMAKE_CXX_STANDARD 20)

set(NAME "Chicane")

project(${NAME} VERSION 1.0.0)

option(BUILD_SHARED_LIBS "Build package with shared libraries." ON)

##################### Vendors #######################
find_package(Vulkan REQUIRED)

if (NOT Vulkan_FOUND)
    message(FATAL_ERROR "Vulkan not found")
endif()

##################### Values #######################
## Output
if (NOT DEFINED BUILD_DIR)
    if (NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE Debug)
    endif()

    set(BUILD_DIR "${CMAKE_CURRENT_LIST_DIR}/Bin/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE}")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR})
set(CMAKE_SHARED_LIBRARY_PREFIX    "")

## Base
set(CONTENTS_DIR "${CMAKE_CURRENT_LIST_DIR}/Contents")
set(INCLUDES_DIR "${CMAKE_CURRENT_LIST_DIR}/Includes")
set(SOURCES_DIR  "${CMAKE_CURRENT_LIST_DIR}/Sources")
set(VENDORS_DIR  "${CMAKE_CURRENT_LIST_DIR}/Vendors")

set(INSTALL_INTERFACE_DIR "${CMAKE_INSTALL_INCLUDEDIR}/${NAME}")
set(BUILD_INTERFACE_DIR   "${INCLUDES_DIR}")

## Chicane
set(CHICANE_INCLUDES_DIR "${INCLUDES_DIR}/Chicane")
set(CHICANE_SOURCES_DIR  "${SOURCES_DIR}/Chicane")
set(
    CHICANE_INCLUDE_DIRS

    ${BOX_INCLUDE_DIRS}
    ${CORE_INCLUDE_DIRS}
    ${RUNTIME_INCLUDE_DIRS}
)

## Box
set(BOX_TARGET_NAME "Box")
set(BOX_HEADER_DIR  "${CHICANE_INCLUDES_DIR}/Box")
set(BOX_SOURCE_DIR  "${CHICANE_SOURCES_DIR}/Box")
set(
    BOX_INCLUDE_DIRS

    # Vendors
    ${VENDORS_DIR}
    ${VENDORS_DIR}/fast_obj
)

## Core
set(CORE_TARGET_NAME "Core")
set(CORE_HEADER_DIR  "${CHICANE_INCLUDES_DIR}/Core")
set(CORE_SOURCE_DIR  "${CHICANE_SOURCES_DIR}/Core")
set(
    CORE_INCLUDE_DIRS

    # Vendors
    ${VENDORS_DIR}
    ${VENDORS_DIR}/base64
    ${VENDORS_DIR}/glm
    ${VENDORS_DIR}/pugixml/src
    ${VENDORS_DIR}/SDL3/include
)

## Grid
set(GRID_TARGET_NAME "Grid")
set(GRID_HEADER_DIR  "${CHICANE_INCLUDES_DIR}/Grid")
set(GRID_SOURCE_DIR  "${CHICANE_SOURCES_DIR}/Grid")

## Runtime
set(RUNTIME_TARGET_NAME "Runtime")
set(RUNTIME_HEADER_DIR  "${CHICANE_INCLUDES_DIR}/Runtime")
set(RUNTIME_SOURCE_DIR  "${CHICANE_SOURCES_DIR}/Runtime")
set(
    RUNTIME_INCLUDE_DIRS

    # Vendors
    ${VENDORS_DIR}
    ${Vulkan_INCLUDE_DIRS}
)

set(RUNTIME_CONTENT_TARGET_NAME "${RUNTIME_TARGET_NAME}_Content")
set(RUNTIME_CONTENT_SOURCE_DIR  "${CONTENTS_DIR}/Runtime")

set(RUNTIME_CONTENT_VULKAN_TARGET_NAME "${RUNTIME_CONTENT_TARGET_NAME}_Vulkan")
set(RUNTIME_CONTENT_VULKAN_SOURCE_DIR  "${RUNTIME_CONTENT_SOURCE_DIR}/Vulkan")

set(RUNTIME_CONTENT_VULKAN_SHADERS_TARGET_NAME "${RUNTIME_CONTENT_VULKAN_TARGET_NAME}_Shaders")
set(RUNTIME_CONTENT_VULKAN_SHADERS_SOURCE_DIR  "${RUNTIME_CONTENT_VULKAN_SOURCE_DIR}/Shaders")

##################### Directories #######################
add_subdirectory(Contents)
add_subdirectory(Sources)
add_subdirectory(Vendors)
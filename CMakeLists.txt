##################### Includes #######################
include(CMakePackageConfigHelpers)

include(.env.cmake OPTIONAL RESULT_VARIABLE LOCAL_ENV)

include(Functions.cmake)

##################### Settings #######################
cmake_minimum_required(VERSION 3.16.0)

set(CMAKE_CXX_STANDARD          17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(NAME "Chicane")

project(${NAME} VERSION 0.1.0)

set(BUILD_SHARED_LIBS ON)

add_definitions(-DBUILD_SHARED)

if (CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    add_definitions(-DBUILD_EXPORT)
endif()

# SDL3 Setup
set(SDL3_DISABLE_INSTALL ON  CACHE BOOL "" FORCE)
set(SDL3_TESTS           OFF CACHE BOOL "" FORCE)

##################### Values #######################
if (NOT DEFINED BUILD_DIR)
    if (NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE Debug)
    endif()

    set(BUILD_DIR "${CMAKE_CURRENT_LIST_DIR}/Bin/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE}")
endif()

## System
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR})

set(CMAKE_SHARED_LIBRARY_PREFIX "")

## Base
set(CONTENTS_DIR "${CMAKE_CURRENT_LIST_DIR}/Contents")
set(INCLUDES_DIR "${CMAKE_CURRENT_LIST_DIR}/Includes")
set(SOURCES_DIR  "${CMAKE_CURRENT_LIST_DIR}/Sources")
set(VENDORS_DIR  "${CMAKE_CURRENT_LIST_DIR}/Vendors")

set(BUILD_INTERFACE_DIR   "${INCLUDES_DIR}/${NAME}")
set(INSTALL_INTERFACE_DIR "${CMAKE_INSTALL_INCLUDEDIR}")

## Chicane
set(CHICANE_INCLUDES_DIR "${INCLUDES_DIR}/Chicane")
set(CHICANE_SOURCES_DIR  "${SOURCES_DIR}/Chicane")
set(CHICANE_INSTALL_DIR  "${CMAKE_INSTALL_INCLUDEDIR}/Chicane")

## Vendors
set(VENDORS_INSTALL_DIR "${CMAKE_INSTALL_INCLUDEDIR}/Vendors")

## Box
set(BOX_TARGET_NAME "${NAME}-Box")
set(BOX_HEADER_DIR  "${CHICANE_INCLUDES_DIR}/Box")
set(BOX_SOURCE_DIR  "${CHICANE_SOURCES_DIR}/Box")

## Core
set(CORE_TARGET_NAME "${NAME}-Core")
set(CORE_HEADER_DIR  "${CHICANE_INCLUDES_DIR}/Core")
set(CORE_SOURCE_DIR  "${CHICANE_SOURCES_DIR}/Core")

## Grid
set(GRID_TARGET_NAME "${NAME}-Grid")
set(GRID_HEADER_DIR  "${CHICANE_INCLUDES_DIR}/Grid")
set(GRID_SOURCE_DIR  "${CHICANE_SOURCES_DIR}/Grid")

## Runtime
set(RUNTIME_TARGET_NAME "${NAME}-Runtime")
set(RUNTIME_HEADER_DIR  "${CHICANE_INCLUDES_DIR}/Runtime")
set(RUNTIME_SOURCE_DIR  "${CHICANE_SOURCES_DIR}/Runtime")

set(RUNTIME_CONTENT_TARGET_NAME "${RUNTIME_TARGET_NAME}-Content")
set(RUNTIME_CONTENT_SOURCE_DIR  "${CONTENTS_DIR}/Runtime")

set(RUNTIME_CONTENT_VULKAN_TARGET_NAME "${RUNTIME_CONTENT_TARGET_NAME}-Vulkan")
set(RUNTIME_CONTENT_VULKAN_SOURCE_DIR  "${RUNTIME_CONTENT_SOURCE_DIR}/Vulkan")

set(RUNTIME_CONTENT_VULKAN_SHADERS_TARGET_NAME "${RUNTIME_CONTENT_VULKAN_TARGET_NAME}-Shaders")
set(RUNTIME_CONTENT_VULKAN_SHADERS_SOURCE_DIR  "${RUNTIME_CONTENT_VULKAN_SOURCE_DIR}/Shaders")

##################### Directories #######################
add_subdirectory(Contents)
add_subdirectory(Sources)
add_subdirectory(Vendors)
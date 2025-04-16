##################### Includes #######################
include(.env.cmake OPTIONAL RESULT_VARIABLE LOCAL_ENV)
include(Functions.cmake)
include(CMakePackageConfigHelpers)

##################### Settings #######################
cmake_minimum_required(VERSION 3.11.0)

set(CMAKE_CXX_STANDARD          20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(NAME "Chicane")

project(${NAME} VERSION 0.1.0)

option(BUILD_SHARED_LIBS "Build package with shared libraries." ON)

if (BUILD_SHARED_LIBS)
    add_definitions(-DBUILD_SHARED)
    add_definitions(-DBUILD_SHARED_DLL)
endif()

##################### Vendors #######################
find_package(Vulkan REQUIRED)

if (NOT Vulkan_FOUND)
    message(FATAL_ERROR "Vulkan not found")
endif()

##################### Values #######################
if (NOT DEFINED BUILD_DIR)
    if (NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE Debug)
    endif()

    set(BUILD_DIR "${CMAKE_CURRENT_LIST_DIR}/Bin/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE}")
endif()

## System
set(CMAKE_STATIC_LIBRARY_PREFIX "")
set(CMAKE_SHARED_LIBRARY_PREFIX "")

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
set(CHICANE_INSTALL_DIR  "${CMAKE_INSTALL_INCLUDEDIR}/Chicane")

## Box
set(BOX_TARGET_NAME "${NAME}-Box")
set(BOX_HEADER_DIR  "${CHICANE_INCLUDES_DIR}/Box")
set(BOX_SOURCE_DIR  "${CHICANE_SOURCES_DIR}/Box")
set(
    BOX_INCLUDE_DIRS

    # Vendors
    ${VENDORS_DIR}/fast_obj
    ${VENDORS_DIR}/glm
    ${VENDORS_DIR}/pugixml/src
    ${VENDORS_DIR}/SDL3/include
    ${VENDORS_DIR}/stb
)
set(
    BOX_LIBRARIES

    # Systems
    Chicane::Core

    # Vendors
    pugixml::pugixml
    SDL3::SDL3
)

## Core
set(CORE_TARGET_NAME "${NAME}-Core")
set(CORE_HEADER_DIR  "${CHICANE_INCLUDES_DIR}/Core")
set(CORE_SOURCE_DIR  "${CHICANE_SOURCES_DIR}/Core")
set(
    CORE_INCLUDE_DIRS

    # Vendors
    ${VENDORS_DIR}/glm
    ${VENDORS_DIR}/pugixml/src
    ${VENDORS_DIR}/SDL3/include
    ${VENDORS_DIR}/stb
)
set(
    CORE_LIBRARIES

    # Vendors
    pugixml::pugixml
    SDL3::SDL3
)

## Grid
set(GRID_TARGET_NAME "${NAME}-Grid")
set(GRID_HEADER_DIR  "${CHICANE_INCLUDES_DIR}/Grid")
set(GRID_SOURCE_DIR  "${CHICANE_SOURCES_DIR}/Grid")
set(
    GRID_INCLUDE_DIRS

    # Vendors
    ${VENDORS_DIR}/fast_obj
    ${VENDORS_DIR}/glm
    ${VENDORS_DIR}/pugixml/src
    ${VENDORS_DIR}/SDL3/include
    ${VENDORS_DIR}/stb
)
set(
    GRID_LIBRARIES

    # Systems
    Chicane::Box
    Chicane::Core

    # Vendors
    pugixml::pugixml
    SDL3::SDL3
)

## Runtime
set(RUNTIME_TARGET_NAME "${NAME}-Runtime")
set(RUNTIME_HEADER_DIR  "${CHICANE_INCLUDES_DIR}/Runtime")
set(RUNTIME_SOURCE_DIR  "${CHICANE_SOURCES_DIR}/Runtime")
set(
    RUNTIME_INCLUDE_DIRS

    # Vendors
    ${VENDORS_DIR}/fast_obj
    ${VENDORS_DIR}/glm
    ${VENDORS_DIR}/pugixml/src
    ${VENDORS_DIR}/SDL3/include
    ${VENDORS_DIR}/stb
    ${Vulkan_INCLUDE_DIRS}
)
set(
    RUNTIME_LIBRARIES

    # Systems
    Chicane::Box
    Chicane::Core
    Chicane::Grid

    # Vendors
    pugixml::pugixml
    SDL3::SDL3
    Vulkan::Vulkan
)

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

##################### Install #######################
CH_INSTALL_FILES("${CHICANE_INCLUDES_DIR}/*.hpp" ${CHICANE_INCLUDES_DIR} ${CHICANE_INSTALL_DIR})
CH_INSTALL_FILES("${BUILD_DIR}/*" ${BUILD_DIR} ${CMAKE_INSTALL_BINDIR})
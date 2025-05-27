##################### Includes #######################
include(CMakePackageConfigHelpers)

include(.env.cmake OPTIONAL RESULT_VARIABLE LOCAL_ENV)

include(Functions.cmake)

##################### Settings #######################
cmake_minimum_required(VERSION 3.16.0)

set(CMAKE_CXX_STANDARD          17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(NAME "Chicane")

project(${NAME} VERSION 0.1.0)

##################### Options #######################
option(CHICANE_SHARED "Build a shared version of the library" BUILD_SHARED_LIBS)

##################### Values #######################
if (CHICANE_SHARED)
    add_definitions(-DBUILD_SHARED)
endif()

if (NOT DEFINED BUILD_DIR)
    if (NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE Debug)
    endif()

    set(BUILD_DIR "${CMAKE_CURRENT_LIST_DIR}/Bin/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE}")
endif()

## System
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG   "${BUILD_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${BUILD_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${BUILD_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${BUILD_DIR}")

set(CMAKE_SHARED_LIBRARY_PREFIX "")

## Base
set(INCLUDES_DIR "${CMAKE_CURRENT_LIST_DIR}/Includes")
set(SOURCES_DIR  "${CMAKE_CURRENT_LIST_DIR}/Sources")

set(BUILD_INTERFACE_DIR   "${INCLUDES_DIR}")
set(INSTALL_INTERFACE_DIR "${CMAKE_INSTALL_INCLUDEDIR}")

## Chicane
set(CHICANE_INCLUDES_DIR "${INCLUDES_DIR}/Chicane")
set(CHICANE_SOURCES_DIR  "${SOURCES_DIR}/Chicane")

## Contents
set(CONTENTS_TARGET_NAME "${NAME}-Contents")
set(CONTENTS_SOURCE_DIR  "${CMAKE_CURRENT_LIST_DIR}/Contents")

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

##################### Directories #######################
add_subdirectory(Contents)
add_subdirectory(Sources)
add_subdirectory(Vendors)
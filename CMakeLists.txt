############## Include #######################
include(./.env.cmake OPTIONAL RESULT_VARIABLE LOCAL_ENV)

############## Setup Project #######################
cmake_minimum_required(VERSION 3.11.0)

set(CMAKE_CXX_STANDARD 17)

set(NAME Chicane)

project(${NAME} VERSION 1.0.0)

##############  Set values  #######################
set(BUILD_DIR "${CMAKE_SOURCE_DIR}/Build/Deploy")

# Engine
set(ENGINE_TARGET_NAME "Engine")

set(ENGINE_BUILD_DIR "${BUILD_DIR}/Engine")

set(ENGINE_DIR "${PROJECT_SOURCE_DIR}/Engine")

set(ENGINE_SOURCE_DIR  "${ENGINE_DIR}/Source")
set(ENGINE_VENDOR_DIR  "${ENGINE_DIR}/Vendor")

set(ENGINE_INCLUDE_STB_PATH    "${ENGINE_VENDOR_DIR}/stb")
set(ENGINE_INCLUDE_VULKAN_PATH "${ENGINE_VENDOR_DIR}/vulkan/Include")

set(ENGINE_LIBRARY_VULKAN_PATH "${ENGINE_VENDOR_DIR}/vulkan/Lib")

# Editor
set(EDITOR_TARGET_NAME "Editor")

set(EDITOR_BUILD_DIR "${BUILD_DIR}/Editor")

set(EDITOR_DIR "${PROJECT_SOURCE_DIR}/Editor")

set(EDITOR_SOURCE_DIR "${EDITOR_DIR}/Source")

##############  Set compiler  #######################
message(STATUS "using ${CMAKE_GENERATOR}")

if (CMAKE_GENERATOR STREQUAL "MinGW Makefiles")
    if (NOT MINGW_PATH) 
      	message(FATAL_ERROR "MINGW_PATH not set in .env.cmake")
    endif()

    set(USE_MINGW "True")
    set(CMAKE_C_COMPILER    ${MINGW_PATH}/bin/gcc.exe)
    set(CMAKE_CXX_COMPILER  ${MINGW_PATH}/bin/g++.exe)
endif()

##############  Add subdirectories  #######################

add_subdirectory(Engine)
add_subdirectory(Editor)
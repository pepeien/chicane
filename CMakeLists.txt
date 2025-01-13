############## Include #######################
include(./.env.cmake OPTIONAL RESULT_VARIABLE LOCAL_ENV)

############## Setup Project #######################
cmake_minimum_required(VERSION 3.11.0)

set(CMAKE_CXX_STANDARD 17)

set(NAME "Chicane")

project(${NAME} VERSION 0.0.1)

##############  Set values  #######################
if (NOT DEFINED BUILD_DIR)
    if (NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE Debug)
    endif()

    set(BUILD_DIR "${CMAKE_CURRENT_LIST_DIR}/Bin/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE}")
endif()

set(CONTENT_DIR "${CMAKE_CURRENT_LIST_DIR}/Content")
set(SOURCE_DIR  "${CMAKE_CURRENT_LIST_DIR}/Source")
set(VENDORS_DIR "${CMAKE_CURRENT_LIST_DIR}/Vendors")

set(PUBLIC_DIR  "${SOURCE_DIR}/Public")
set(PRIVATE_DIR "${SOURCE_DIR}/Private")

## Runtime
set(RUNTIME_TARGET_NAME "${NAME}_Runtime")
set(RUNTIME_SOURCE_DIR  "${PRIVATE_DIR}/Chicane")

set(RUNTIME_CONTENT_TARGET_NAME "${RUNTIME_TARGET_NAME}_Content")
set(RUNTIME_CONTENT_SOURCE_DIR  "${CONTENT_DIR}/Engine")

set(RUNTIME_CONTENT_VULKAN_TARGET_NAME "${RUNTIME_CONTENT_TARGET_NAME}_Vulkan")
set(RUNTIME_CONTENT_VULKAN_SOURCE_DIR  "${RUNTIME_CONTENT_SOURCE_DIR}/Vulkan")

set(RUNTIME_CONTENT_VULKAN_SHADERS_TARGET_NAME "${RUNTIME_CONTENT_VULKAN_TARGET_NAME}_Shaders")
set(RUNTIME_CONTENT_VULKAN_SHADERS_SOURCE_DIR  "${RUNTIME_CONTENT_VULKAN_SOURCE_DIR}/Shaders")

## Sample
set(SAMPLE_TARGET_NAME "${NAME}_Sample")
set(SAMPLE_SOURCE_DIR  "${PRIVATE_DIR}/Sample")

set(SAMPLE_CONTENT_TARGET_NAME "${SAMPLE_TARGET_NAME}_Content")
set(SAMPLE_CONTENT_SOURCE_DIR "${CONTENT_DIR}/Sample")

message("${SAMPLE_CONTENT_SOURCE_DIR}/")

############## Setup Libs #######################
find_package(Vulkan REQUIRED)

if (NOT Vulkan_FOUND)
    message(FATAL_ERROR "Vulkan not found")
endif()

##############  Add subdirectories  #######################
add_subdirectory(Content)
add_subdirectory(Source)
add_subdirectory(Vendors)
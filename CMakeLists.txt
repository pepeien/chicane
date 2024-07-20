############## Include #######################
include(./.env.cmake OPTIONAL RESULT_VARIABLE LOCAL_ENV)

############## Setup Project #######################
cmake_minimum_required(VERSION 3.11.0)

set(CMAKE_CXX_STANDARD 17)

set(NAME "Chicane")

project(${NAME} VERSION 0.0.1)

##############  Set values  #######################
set(BUILD_DIR    "${CMAKE_CURRENT_SOURCE_DIR}/Bin/${CMAKE_SYSTEM_NAME}")

set(ASSETS_DIR   "${CMAKE_CURRENT_SOURCE_DIR}/Assets")
set(CONTENT_DIR  "${CMAKE_CURRENT_SOURCE_DIR}/Content")
set(INCLUDES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Includes")
set(SOURCES_DIR  "${CMAKE_CURRENT_SOURCE_DIR}/Sources")
set(VENDORS_DIR  "${CMAKE_CURRENT_SOURCE_DIR}/Vendors")

set(CONTENT_TARGET_NAME "Content")
set(CONTENT_SOURCE_DIR  "${CONTENT_DIR}")

set(RUNTIME_TARGET_NAME "Runtime")
set(RUNTIME_SOURCE_DIR  "${SOURCES_DIR}/Runtime")

set(EDITOR_TARGET_NAME "Editor")
set(EDITOR_SOURCE_DIR  "${SOURCES_DIR}/Editor")

set(SHADERS_TARGET_NAME "Shaders")
set(SHADERS_SOURCE_DIR  "${ASSETS_DIR}/Shaders")

############## Setup Libs #######################
find_package(Vulkan REQUIRED)

if (NOT Vulkan_FOUND)
    message(FATAL_ERROR "Vulkan not found")
endif()

##############  Add subdirectories  #######################
add_subdirectory(Content)

add_subdirectory(Assets/Shaders)

add_subdirectory(Sources/Editor)
add_subdirectory(Sources/Runtime)

add_subdirectory(Vendors/pugixml)
add_subdirectory(Vendors/SDL2)
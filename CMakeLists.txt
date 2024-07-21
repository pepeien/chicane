############## Include #######################
include(./.env.cmake OPTIONAL RESULT_VARIABLE LOCAL_ENV)

############## Setup Project #######################
cmake_minimum_required(VERSION 3.11.0)

set(CMAKE_CXX_STANDARD 17)

set(NAME "Chicane")

project(${NAME} VERSION 0.0.1)

##############  Set values  #######################
if (NOT DEFINED BUILD_DIR)
    set(BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Bin/${CMAKE_SYSTEM_NAME}")
endif()

set(ASSETS_DIR   "${CMAKE_CURRENT_LIST_DIR}/Assets")
set(CONTENT_DIR  "${CMAKE_CURRENT_LIST_DIR}/Content")
set(INCLUDES_DIR "${CMAKE_CURRENT_LIST_DIR}/Includes")
set(SOURCES_DIR  "${CMAKE_CURRENT_LIST_DIR}/Sources")
set(VENDORS_DIR  "${CMAKE_CURRENT_LIST_DIR}/Vendors")

set(ASSETS_TARGET_NAME "${NAME}_Assets")
set(ASSETS_SOURCE_DIR  "${ASSETS_DIR}")

set(CONTENT_TARGET_NAME "${NAME}_Content")
set(CONTENT_SOURCE_DIR  "${CONTENT_DIR}")

############## Setup Libs #######################
find_package(Vulkan REQUIRED)

if (NOT Vulkan_FOUND)
    message(FATAL_ERROR "Vulkan not found")
endif()

##############  Add subdirectories  #######################
add_subdirectory(Assets)
add_subdirectory(Content)

add_subdirectory(Sources)

add_subdirectory(Vendors/pugixml)
add_subdirectory(Vendors/SDL2)
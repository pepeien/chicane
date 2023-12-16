############## Include #######################
include(./.env.cmake OPTIONAL RESULT_VARIABLE LOCAL_ENV)

############## Setup Project #######################
cmake_minimum_required(VERSION 3.11.0)

set(CMAKE_CXX_STANDARD 17)

set(NAME Chicane)

project(${NAME} VERSION 1.0.0)

##############  Set values  #######################
set(SHADERS_TARGET_NAME "Shaders")
set(RUNTIME_TARGET_NAME "Runtime")
set(EDITOR_TARGET_NAME  "Editor")

set(ENGINE_SOURCE_DIR "${PROJECT_SOURCE_DIR}/Engine")
set(LIBS_DIR          "${PROJECT_SOURCE_DIR}/Libs")

set(SHADERS_SOURCE_DIR "${ENGINE_SOURCE_DIR}/Shaders")

set(RUNTIME_SOURCE_DIR "${ENGINE_SOURCE_DIR}/Source/Runtime")
set(EDITOR_SOURCE_DIR  "${ENGINE_SOURCE_DIR}/Source/Editor")

set(GLFW_PATH   "${LIBS_DIR}/glfw")
set(VULKAN_PATH "${LIBS_DIR}/vulkan")
set(SDL_PATH    "${VULKAN_PATH}/include/SDL2")
set(GLM_PATH    "${LIBS_DIR}/glm")

set(BUILD_DIR         "${CMAKE_SOURCE_DIR}/Build")
set(BUILD_DEPLOY_DIR  "${BUILD_DIR}/Deploy")
set(BUILD_BINARY_DIR  "${BUILD_DEPLOY_DIR}/Binaries")
set(BUILD_CONTENT_DIR "${BUILD_DEPLOY_DIR}/Content")

##############  Set compiler  #######################
message(STATUS "using ${CMAKE_GENERATOR}")

if (CMAKE_GENERATOR STREQUAL "MinGW Makefiles")
    if (NOT MINGW_PATH) 
      	message(FATAL_ERROR "MINGW_PATH not set in .env.cmake")
    endif()

    set(USE_MINGW "True")
    set(CMAKE_C_COMPILER ${MINGW_PATH}/bin/gcc.exe)
    set(CMAKE_CXX_COMPILER  ${MINGW_PATH}/bin/g++.exe)
endif()

##############  Add subdirectories  #######################

add_subdirectory(Engine/Shaders)

add_subdirectory(Engine/Source/Runtime)
add_subdirectory(Engine/Source/Editor)
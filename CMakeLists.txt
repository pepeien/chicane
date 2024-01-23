############## Include #######################
include(./.env.cmake OPTIONAL RESULT_VARIABLE LOCAL_ENV)

############## Setup Project #######################
cmake_minimum_required(VERSION 3.11.0)

set(CMAKE_CXX_STANDARD 17)

set(NAME Chicane)

project(${NAME} VERSION 0.0.1)

##############  Set values  #######################
set(BUILD_DIR   "${CMAKE_SOURCE_DIR}/Bin/${CMAKE_SYSTEM_NAME}")
set(SOURCES_DIR "${CMAKE_SOURCE_DIR}/Sources")
set(VENDOR_DIR  "${SOURCES_DIR}/Vendor")

# Runtime
set(RUNTIME_TARGET_NAME "Runtime")
set(RUNTIME_SOURCES_DIR "${SOURCES_DIR}/Runtime")
set(RUNTIME_BUILD_DIR   "${BUILD_DIR}")

# Shader
set(SHADER_TARGET_NAME "Shader")
set(SHADER_SOURCES_DIR "${CMAKE_SOURCE_DIR}/Shaders")
set(SHADER_BUILD_DIR   "${SHADER_SOURCES_DIR}/Bin")

##############  Set compiler  #######################
message(STATUS "Using ${CMAKE_GENERATOR}")

if (CMAKE_GENERATOR STREQUAL "MinGW Makefiles")
    if (NOT MINGW_PATH) 
      	message(FATAL_ERROR "MINGW_PATH not set in .env.cmake")
    endif()

    set(USE_MINGW          "True")
    set(CMAKE_C_COMPILER   ${MINGW_PATH}/bin/gcc.exe)
    set(CMAKE_CXX_COMPILER ${MINGW_PATH}/bin/g++.exe)
endif()

##############  Add subdirectories  #######################
 
add_subdirectory(Shaders)
add_subdirectory(Sources/Runtime)
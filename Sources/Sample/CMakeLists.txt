##################### Includes #######################
include(./.env.cmake OPTIONAL RESULT_VARIABLE LOCAL_ENV)

##################### Settings #######################
cmake_minimum_required(VERSION 3.11.0)

set(CMAKE_CXX_STANDARD          20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(NAME "Samples")

project(${NAME} VERSION 1.0.0)

##################### Vendors #######################
find_package(Vulkan REQUIRED)

if (NOT Vulkan_FOUND)
    message(FATAL_ERROR "Vulkan not found")
endif()

##################### Functions #######################
function(CH_ADD_COMPILER TARGET_NAME)
    if (CMAKE_GENERATOR STREQUAL "MinGW Makefiles")
        if (NOT MINGW_PATH) 
              message(FATAL_ERROR "MINGW_PATH not set in .env.cmake")
        endif()

        set(CMAKE_C_COMPILER   ${MINGW_PATH}/bin/gcc.exe)
        set(CMAKE_CXX_COMPILER ${MINGW_PATH}/bin/g++.exe)

        if (WIN32)
            target_include_directories(
                ${TARGET_NAME}

                PRIVATE
                    ${MINGW_PATH}/include
            )

            target_link_directories(
                ${TARGET_NAME}

                PRIVATE
                    ${MINGW_PATH}/lib
            )
        endif()
    endif()
endfunction()

##################### Values #######################
## Output
if (NOT DEFINED BUILD_DIR)
    if (NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE Debug)
    endif()

    set(BUILD_DIR "${CMAKE_CURRENT_LIST_DIR}/Bin/${CMAKE_SYSTEM_NAME}/${CMAKE_BUILD_TYPE}")
endif()

## Base
set(CONTENTS_DIR "${CMAKE_CURRENT_LIST_DIR}/Contents")
set(SOURCES_DIR  "${CMAKE_CURRENT_LIST_DIR}/Sources")

## Shooter
set(SHOOTER_TARGET_NAME "Shooter")
set(SHOOTER_HEADER_DIR  "${SOURCES_DIR}/Shooter")
set(SHOOTER_SOURCE_DIR  "${SOURCES_DIR}/Shooter")

set(SHOOTER_CONTENT_TARGET_NAME "${SHOOTER_TARGET_NAME}_Content")
set(SHOOTER_CONTENT_SOURCE_DIR  "${CONTENTS_DIR}/Shooter")

##################### Directories #######################
add_subdirectory(Contents)
add_subdirectory(Sources)
############## Include #######################
include(./.env.cmake OPTIONAL RESULT_VARIABLE LOCAL_ENV)

############## Setup Project #######################
cmake_minimum_required(VERSION 3.11.0)

set(CMAKE_CXX_STANDARD 17)

set(NAME "Chicane")

project(${NAME} VERSION 0.0.1)

##############  Set values  #######################
set(TARGET_NAME         "Chicane")
set(TARGET_NAME_STATIC  "${TARGET_NAME}-static")
set(SHADERS_TARGET_NAME "Shaders")

set(SOURCES_DIR "${CMAKE_SOURCE_DIR}/Sources")
set(BUILD_DIR   "${CMAKE_SOURCE_DIR}/Bin/${CMAKE_SYSTEM_NAME}")
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/Includes")

##############  Set compiler  #######################
message(STATUS "Using ${CMAKE_GENERATOR}")

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

##############  Add subdirectories  #######################
add_subdirectory(Shaders)

add_subdirectory(Vendors/pugixml)
add_subdirectory(Vendors/SDL2)

##############  Set values  #######################
set(INCLUDES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Includes")
set(SOURCES_DIR  "${CMAKE_CURRENT_SOURCE_DIR}/Sources")
set(VENDORS_DIR  "${CMAKE_CURRENT_SOURCE_DIR}/Vendors")

##############  Set source files  #######################
file(
    GLOB_RECURSE
    SOURCES

    ${SOURCES_DIR}/*.cpp

    ${VENDORS_DIR}/imgui/imgui_draw.cpp
    ${VENDORS_DIR}/imgui/imgui_tables.cpp
    ${VENDORS_DIR}/imgui/imgui_widgets.cpp
    ${VENDORS_DIR}/imgui/imgui.cpp
    ${VENDORS_DIR}/imgui/backends/imgui_impl_sdl2.cpp
    ${VENDORS_DIR}/imgui/backends/imgui_impl_vulkan.cpp
)

############## Setup Libs #######################
find_package(Vulkan REQUIRED)

if (NOT Vulkan_FOUND)
    message(FATAL_ERROR "Vulkan not found")
endif()

############## Setup Static #######################
add_library(
    ${TARGET_NAME_STATIC}
    STATIC
    ""
)

target_sources(
    ${TARGET_NAME_STATIC}

    PRIVATE
        ${SOURCES}
)

target_include_directories(
    ${TARGET_NAME_STATIC}

    PUBLIC
        $<INSTALL_INTERFACE:Includes>
        $<BUILD_INTERFACE:${INCLUDES_DIR}>

        ${Vulkan_INCLUDE_DIRS}
        ${VENDORS_DIR}
        ${VENDORS_DIR}/glm
        ${VENDORS_DIR}/imgui
        ${VENDORS_DIR}/pugixml
        ${VENDORS_DIR}/SDL2
    PRIVATE
        ${SOURCES_DIR}
)

target_link_libraries(
    ${TARGET_NAME_STATIC}

    PUBLIC
        pugixml-static
        SDL2-static
    PRIVATE
        ${Vulkan_LIBRARIES}
)

############## Setup Shared #######################

add_library(
    ${TARGET_NAME}
    SHARED
    ""
)

target_sources(
    ${TARGET_NAME}

    PRIVATE
        ${SOURCES}
)

set_target_properties(
    ${TARGET_NAME}
    PROPERTIES

    VS_DEBUGGER_WORKING_DIRECTORY
    "${BUILD_DIR}"

    RUNTIME_OUTPUT_DIRECTORY
    "${BUILD_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG
    "${BUILD_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE
    "${BUILD_DIR}"

    LIBRARY_OUTPUT_DIRECTORY
    "${BUILD_DIR}"
    LIBRARY_OUTPUT_DIRECTORY_DEBUG
    "${BUILD_DIR}"
    LIBRARY_OUTPUT_DIRECTORY_RELEASE
    "${BUILD_DIR}"
)

target_include_directories(
    ${TARGET_NAME}

    PUBLIC
        $<INSTALL_INTERFACE:Includes>
        $<BUILD_INTERFACE:${INCLUDES_DIR}>

        ${Vulkan_INCLUDE_DIRS}
        ${VENDORS_DIR}
        ${VENDORS_DIR}/glm
        ${VENDORS_DIR}/imgui
        ${VENDORS_DIR}/pugixml
        ${VENDORS_DIR}/SDL2
    PRIVATE
        ${SOURCES_DIR}
)

target_link_libraries(
    ${TARGET_NAME}

    PUBLIC
        pugixml-static
        SDL2-static
    PRIVATE
        ${Vulkan_LIBRARIES}
)
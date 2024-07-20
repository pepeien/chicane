##############  Set compiler  #######################
if (CMAKE_GENERATOR STREQUAL "MinGW Makefiles")
    if (NOT MINGW_PATH) 
      	message(FATAL_ERROR "MINGW_PATH not set in .env.cmake")
    endif()

    set(CMAKE_C_COMPILER   ${MINGW_PATH}/bin/gcc.exe)
    set(CMAKE_CXX_COMPILER ${MINGW_PATH}/bin/g++.exe)

    if (WIN32)
        target_include_directories(
            ${RUNTIME_TARGET_NAME}

            PRIVATE
                ${MINGW_PATH}/include
        )

        target_link_directories(
            ${RUNTIME_TARGET_NAME}

            PRIVATE
                ${MINGW_PATH}/lib
        )
    endif()
endif()

##############  Set source files  #######################
file(
    GLOB_RECURSE
    SOURCES

    ${RUNTIME_SOURCE_DIR}/*.cpp

    ${VENDORS_DIR}/imgui/imgui_draw.cpp
    ${VENDORS_DIR}/imgui/imgui_tables.cpp
    ${VENDORS_DIR}/imgui/imgui_widgets.cpp
    ${VENDORS_DIR}/imgui/imgui.cpp
    ${VENDORS_DIR}/imgui/misc/cpp/imgui_stdlib.cpp
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
    ${RUNTIME_TARGET_NAME}
    STATIC
    ${SOURCES}
)

add_library(
    "${NAME}::Runtime"

    ALIAS
        ${RUNTIME_TARGET_NAME}
)

set_target_properties(
    ${RUNTIME_TARGET_NAME}
    PROPERTIES

    OUTPUT_NAME
    "Chicane"

    VS_DEBUGGER_WORKING_DIRECTORY
    "${BUILD_DIR}"

    ARCHIVE_OUTPUT_DIRECTORY
    "${BUILD_DIR}"

    ARCHIVE_OUTPUT_DIRECTORY_DEBUG
    "${BUILD_DIR}"

    ARCHIVE_OUTPUT_DIRECTORY_RELEASE
    "${BUILD_DIR}"
)

target_include_directories(
    ${RUNTIME_TARGET_NAME}

    PUBLIC
        $<BUILD_INTERFACE:${INCLUDES_DIR}>
        $<INSTALL_INTERFACE:Includes>

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
    ${RUNTIME_TARGET_NAME}

    PUBLIC
        pugixml-static
        SDL2-static
    PRIVATE
        ${Vulkan_LIBRARIES}
)

add_dependencies(${RUNTIME_TARGET_NAME} ${ASSETS_TARGET_NAME})
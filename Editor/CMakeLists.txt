##############  Set Values  #######################
set(TARGET_NAME "${EDITOR_TARGET_NAME}")
set(SOURCES_DIR "${EDITOR_SOURCE_DIR}")

##############  Set Source Files  #######################
file(
    GLOB_RECURSE
    SOURCES

    ${SOURCES_DIR}/**.cpp

    # Vendor
    ${EDITOR_INCLUDE_IMGUI_PATH}/imgui.cpp
    ${EDITOR_INCLUDE_IMGUI_PATH}/imgui_draw.cpp
    ${EDITOR_INCLUDE_IMGUI_PATH}/imgui_tables.cpp
    ${EDITOR_INCLUDE_IMGUI_PATH}/imgui_widgets.cpp
    ${EDITOR_INCLUDE_IMGUI_PATH}/backends/imgui_impl_sdl2.cpp
    ${EDITOR_INCLUDE_IMGUI_PATH}/backends/imgui_impl_vulkan.cpp
)

add_executable(${TARGET_NAME} ${SOURCES})

############## Set Property Balues  #######################
set_property(
    TARGET
    ${TARGET_NAME}
    PROPERTY
    VS_DEBUGGER_WORKING_DIRECTORY
    "${EDITOR_BUILD_DIR}"
)

set_property(
    TARGET
    ${TARGET_NAME}
    PROPERTY
    RUNTIME_OUTPUT_DIRECTORY
    "${EDITOR_BUILD_DIR}"
)

set_property(
    TARGET
    ${TARGET_NAME}
    PROPERTY
    RUNTIME_OUTPUT_DIRECTORY_DEBUG
    "${EDITOR_BUILD_DIR}"
)

set_property(
    TARGET
    ${TARGET_NAME}
    PROPERTY
    RUNTIME_OUTPUT_DIRECTORY_RELEASE
    "${EDITOR_BUILD_DIR}"
)

############## Linking Libs  #######################

target_include_directories(
    ${TARGET_NAME}
    PUBLIC

    # Source
    ${SOURCES_DIR}

    # Engine
    ${ENGINE_SOURCE_DIR}

    # Vendor
    ${EDITOR_INCLUDE_IMGUI_PATH}
    ${EDITOR_INCLUDE_IMGUI_PATH}/backends
)

target_link_libraries(
    ${TARGET_NAME}
    PRIVATE

    # Engine
    ${ENGINE_TARGET_NAME}
)

if (WIN32)
    message(STATUS "CREATING BUILD FOR WINDOWS")

    if (USE_MINGW)
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
elseif(UNIX)
    message(STATUS "CREATING BUILD FOR UNIX")

endif()
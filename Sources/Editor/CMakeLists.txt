##############  Set values  #######################
set(CONTENT_TARGET_NAME "EditorContent")
set(CONTENT_SOURCE_DIR  "${EDITOR_SOURCE_DIR}/Content")
set(CONTENT_BUILD_DIR   "${BUILD_DIR}/Content")

##############  Set compiler  #######################
if (CMAKE_GENERATOR STREQUAL "MinGW Makefiles")
    if (NOT MINGW_PATH) 
      	message(FATAL_ERROR "MINGW_PATH not set in .env.cmake")
    endif()

    set(CMAKE_C_COMPILER   ${MINGW_PATH}/bin/gcc.exe)
    set(CMAKE_CXX_COMPILER ${MINGW_PATH}/bin/g++.exe)

    if (WIN32)
        target_include_directories(
            ${EDITOR_TARGET_NAME}

            PRIVATE
                ${MINGW_PATH}/include
        )

        target_link_directories(
            ${EDITOR_TARGET_NAME}

            PRIVATE
                ${MINGW_PATH}/lib
        )
    endif()
endif()

##############  Set source files  #######################
file(
    GLOB_RECURSE
    SOURCES

    ${EDITOR_SOURCE_DIR}/*.cpp
)

############## Setup Executable #######################
add_executable(${EDITOR_TARGET_NAME} "")

target_sources(
    ${EDITOR_TARGET_NAME}

    PRIVATE
        ${SOURCES}
)

set_target_properties(
    ${EDITOR_TARGET_NAME}
    PROPERTIES

    VS_DEBUGGER_WORKING_DIRECTORY
    "${BUILD_DIR}"

    RUNTIME_OUTPUT_DIRECTORY
    "${BUILD_DIR}"

    RUNTIME_OUTPUT_DIRECTORY_DEBUG
    "${BUILD_DIR}"

    RUNTIME_OUTPUT_DIRECTORY_RELEASE
    "${BUILD_DIR}"
)

target_include_directories(
    ${EDITOR_TARGET_NAME}

    PRIVATE
        ${SOURCES_DIR}
)

target_link_libraries(
    ${EDITOR_TARGET_NAME}

    PRIVATE
        Chicane::Runtime
)

##############  Add subdirectories  #######################
add_subdirectory(Content)

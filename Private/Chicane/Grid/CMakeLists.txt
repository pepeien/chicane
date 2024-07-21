##############  Set compiler  #######################
if (CMAKE_GENERATOR STREQUAL "MinGW Makefiles")
    if (NOT MINGW_PATH) 
      	message(FATAL_ERROR "MINGW_PATH not set in .env.cmake")
    endif()

    set(CMAKE_C_COMPILER   ${MINGW_PATH}/bin/gcc.exe)
    set(CMAKE_CXX_COMPILER ${MINGW_PATH}/bin/g++.exe)

    if (WIN32)
        target_include_directories(
            ${GRID_TARGET_NAME}

            PRIVATE
                ${MINGW_PATH}/include
        )

        target_link_directories(
            ${GRID_TARGET_NAME}

            PRIVATE
                ${MINGW_PATH}/lib
        )
    endif()
endif()

##############  Set source files  #######################
file(
    GLOB_RECURSE
    SOURCES

    ${GRID_SOURCE_DIR}/*.cpp
)

############## Setup Static #######################
add_library(
    ${GRID_TARGET_NAME}
    STATIC
    ${SOURCES}
)

add_library(
    "${NAME}::Grid"

    ALIAS
        ${GRID_TARGET_NAME}
)

set_target_properties(
    ${GRID_TARGET_NAME}
    PROPERTIES

    OUTPUT_NAME
    "ChicaneGrid"

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
    ${GRID_TARGET_NAME}

    PUBLIC
        $<BUILD_INTERFACE:${PUBLIC_DIR}>
        $<INSTALL_INTERFACE:Includes>

    PRIVATE
        ${PRIVATE_DIR}
)

target_link_libraries(
    ${GRID_TARGET_NAME}

    PUBLIC
        Chicane::Runtime
)
####################### Values #########################
set(SHOOTER_TARGET_NAME "Sample_Shooter")
set(SHOOTER_SOURCE_DIR  "${SOURCES_DIR}/Sample/Shooter")

set(SHOOTER_CONTENT_TARGET_NAME "${SHOOTER_TARGET_NAME}_Content")
set(SHOOTER_CONTENT_SOURCE_DIR  "${SHOOTER_SOURCE_DIR}/Contents")

######################### Files ##########################
file(
    GLOB_RECURSE
    HEADERS

    ${SHOOTER_SOURCE_DIR}/*.hpp
)

file(
    GLOB_RECURSE
    SOURCES

    ${SHOOTER_SOURCE_DIR}/*.cpp
)

##################### Directories #######################
add_executable(${SHOOTER_TARGET_NAME} ${HEADERS} ${SOURCES})

set_target_properties(
    ${SHOOTER_TARGET_NAME}
    PROPERTIES

    OUTPUT_NAME
    "ShooterSample"

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
    ${SHOOTER_TARGET_NAME}

    PUBLIC
        ${CHICANE_INCLUDE_DIRS}
    PRIVATE
        ${SHOOTER_SOURCE_DIR}/Sources
)

target_link_libraries(
    ${SHOOTER_TARGET_NAME}

    PRIVATE
        Chicane::Runtime
)

if (NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    if (WIN32)
        target_link_options(${SHOOTER_TARGET_NAME} PRIVATE -mwindows)
    endif()
endif()

add_dependencies(${SHOOTER_TARGET_NAME} ${SHOOTER_CONTENT_TARGET_NAME})

##################### Setup #######################
if (CMAKE_GENERATOR STREQUAL "MinGW Makefiles")
    if (NOT MINGW_PATH) 
      	message(FATAL_ERROR "MINGW_PATH not set in .env.cmake")
    endif()

    set(CMAKE_C_COMPILER   ${MINGW_PATH}/bin/gcc.exe)
    set(CMAKE_CXX_COMPILER ${MINGW_PATH}/bin/g++.exe)

    if (WIN32)
        target_include_directories(
            ${SHOOTER_TARGET_NAME}

            PRIVATE
                ${MINGW_PATH}/include
        )

        target_link_directories(
            ${SHOOTER_TARGET_NAME}

            PRIVATE
                ${MINGW_PATH}/lib
        )
    endif()
endif()

##################### Directories #######################
add_subdirectory(Contents)
##############  Set source files  #######################
file(
    GLOB_RECURSE
    SOURCES

    ${SAMPLE_SOURCE_DIR}/*.cpp
)

############## Setup Static #######################
add_executable(${SAMPLE_TARGET_NAME} "")

target_sources(
    ${SAMPLE_TARGET_NAME}

    PRIVATE
        ${SOURCES}
)

set_target_properties(
    ${SAMPLE_TARGET_NAME}
    PROPERTIES

    OUTPUT_NAME
    "ChicaneSample"

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
    ${SAMPLE_TARGET_NAME}

    PRIVATE
        ${SAMPLE_SOURCE_DIR}
)

target_link_libraries(
    ${SAMPLE_TARGET_NAME}

    PRIVATE
        Chicane::Runtime
)

if (NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_link_options(${SAMPLE_TARGET_NAME} PRIVATE -mwindows)
endif()

##############  Set compiler  #######################
if (CMAKE_GENERATOR STREQUAL "MinGW Makefiles")
    if (NOT MINGW_PATH) 
      	message(FATAL_ERROR "MINGW_PATH not set in .env.cmake")
    endif()

    set(CMAKE_C_COMPILER   ${MINGW_PATH}/bin/gcc.exe)
    set(CMAKE_CXX_COMPILER ${MINGW_PATH}/bin/g++.exe)

    if (WIN32)
        target_include_directories(
            ${SAMPLE_TARGET_NAME}

            PRIVATE
                ${MINGW_PATH}/include
        )

        target_link_directories(
            ${SAMPLE_TARGET_NAME}

            PRIVATE
                ${MINGW_PATH}/lib
        )
    endif()
endif()
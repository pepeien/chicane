##############  Set source files  #######################
file(
    GLOB_RECURSE
    HEADERS

    # Sources
    ${GRID_HEADER_DIR}/*.hpp
)

file(
    GLOB_RECURSE
    SOURCES

    # Sources
    ${GRID_SOURCE_DIR}/*.cpp
)

############## Setup Static #######################
add_library(${GRID_TARGET_NAME} SHARED ${HEADERS} ${SOURCES})

add_library(
    "${NAME}::Grid"

    ALIAS
        ${GRID_TARGET_NAME}
)

target_include_directories(
    ${GRID_TARGET_NAME}

    PUBLIC
        # Alias
        $<BUILD_INTERFACE:${BUILD_INTERFACE_DIR}>
        $<INSTALL_INTERFACE:${INSTALL_INTERFACE_DIR}>

        # Systems
        ${CORE_INCLUDE_DIRS}
        ${BOX_INCLUDE_DIRS}
)

target_link_libraries(
    ${GRID_TARGET_NAME}

    PUBLIC
        Chicane::Box
        Chicane::Core
)

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
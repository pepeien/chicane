##############  Set source files  #######################
file(
    GLOB_RECURSE
    HEADERS

    # Sources
    ${RUNTIME_HEADER_DIR}/*.hpp
)

file(
    GLOB_RECURSE
    SOURCES

    # Sources
    ${RUNTIME_SOURCE_DIR}/*.cpp
)

############## Setup Static #######################
add_library(${RUNTIME_TARGET_NAME} SHARED ${HEADERS} ${SOURCES})

add_library(
    "${NAME}::Runtime"

    ALIAS
        ${RUNTIME_TARGET_NAME}
)

target_include_directories(
    ${RUNTIME_TARGET_NAME}

    PUBLIC
        # Alias
        $<BUILD_INTERFACE:${BUILD_INTERFACE_DIR}>
        $<INSTALL_INTERFACE:${INSTALL_INTERFACE_DIR}>

        # Systems
        ${CORE_INCLUDE_DIRS}
        ${BOX_INCLUDE_DIRS}
        ${RUNTIME_INCLUDE_DIRS}
)

target_link_libraries(
    ${RUNTIME_TARGET_NAME}

    PUBLIC
        # Systems
        Chicane::Box
        Chicane::Core
        Chicane::Grid

        # Vendors
        Vulkan::Vulkan
)

add_dependencies(
    ${RUNTIME_TARGET_NAME}

    # Contents
    ${RUNTIME_CONTENT_TARGET_NAME}
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
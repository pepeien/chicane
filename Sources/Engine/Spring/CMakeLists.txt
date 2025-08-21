##################### Files #######################
file(
    GLOB_RECURSE
    HEADERS

    # Headers
    ${SPRING_HEADER_DIR}/*.hpp
)
list(APPEND HEADERS ${CHICANE_INCLUDES_DIR}/Spring.hpp)

file(
    GLOB_RECURSE
    SOURCES

    # Sources
    ${SPRING_SOURCE_DIR}/*.cpp
)

##################### Setup #######################
if (CHICANE_SHARED)
    add_library(${SPRING_TARGET_NAME} SHARED ${HEADERS} ${SOURCES})
else()
    add_library(${SPRING_TARGET_NAME} STATIC ${HEADERS} ${SOURCES})
endif()

add_library(
    "${NAME}::Spring"

    ALIAS
        ${SPRING_TARGET_NAME}
)

set_target_properties(
    ${SPRING_TARGET_NAME}

    PROPERTIES
        OUTPUT_NAME
            "${NAME}"
        VS_DEBUGGER_WORKING_DIRECTORY
            "${BUILD_DIR}"
        DEFINE_SYMBOL
            "BUILD_EXPORT"
        POSITION_INDEPENDENT_CODE
            TRUE
)

target_include_directories(
    ${SPRING_TARGET_NAME}

    PUBLIC
        # Alias
        $<BUILD_INTERFACE:${BUILD_INTERFACE_DIR}>
        $<INSTALL_INTERFACE:${INSTALL_INTERFACE_DIR}>
)

target_link_libraries(
    ${SPRING_TARGET_NAME}

    PUBLIC
        # Systems
        Chicane::Core
)

CH_ADD_COMPILER(${SPRING_TARGET_NAME})
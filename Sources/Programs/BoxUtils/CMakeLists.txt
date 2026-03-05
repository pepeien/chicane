######################### Values #########################
set(BOX_UTILS_TARGET_NAME "${PROGRAMS_TARGET_NAME}-Box")
set(BOX_UTILS_SOURCE_DIR  "${PROGRAMS_SOURCE_DIR}/BoxUtils")

######################### Files ##########################
file(
    GLOB_RECURSE
    HEADERS

    ${BOX_UTILS_SOURCE_DIR}/*.hpp
)

file(
    GLOB_RECURSE
    SOURCES

    ${BOX_UTILS_SOURCE_DIR}/*.cpp
)

######################### Setup #########################
add_executable(${BOX_UTILS_TARGET_NAME} ${HEADERS} ${SOURCES})

set_target_properties(
    ${BOX_UTILS_TARGET_NAME}

    PROPERTIES
        OUTPUT_NAME
            "BoxUtils"
        VS_DEBUGGER_WORKING_DIRECTORY
            "${CHICANE_BUILD_DIR}"
)

target_include_directories(
    ${BOX_UTILS_TARGET_NAME}

    PUBLIC
        $<BUILD_INTERFACE:${BUILD_INTERFACE_DIR}>
        $<INSTALL_INTERFACE:${INSTALL_INTERFACE_DIR}>
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(
    ${BOX_UTILS_TARGET_NAME}

    PRIVATE
        Chicane::Box
)

CH_ADD_COMPILER(${BOX_UTILS_TARGET_NAME})
######################### Values #########################
set(BOX_TARGET_NAME "${PROGRAMS_TARGET_NAME}-Box")
set(BOX_HEADER_DIR  "${PROGRAMS_HEADER_DIR}/Box")
set(BOX_SOURCE_DIR  "${PROGRAMS_SOURCE_DIR}/Box")

######################### Files ##########################
file(
    GLOB_RECURSE
    HEADERS

    ${CMAKE_CURRENT_LIST_DIR}/*.hpp
)

file(
    GLOB_RECURSE
    SOURCES

    ${CMAKE_CURRENT_LIST_DIR}/*.cpp
)

######################### Setup #########################
add_executable(${BOX_TARGET_NAME} ${HEADERS} ${SOURCES})

set_target_properties(
    ${BOX_TARGET_NAME}

    PROPERTIES
        OUTPUT_NAME
            "BoxUtils"
        VS_DEBUGGER_WORKING_DIRECTORY
            "${BUILD_DIR}"
)

target_include_directories(
    ${BOX_TARGET_NAME}

    PUBLIC
        $<BUILD_INTERFACE:${BUILD_INTERFACE_DIR}>
        $<INSTALL_INTERFACE:${INSTALL_INTERFACE_DIR}>
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(
    ${BOX_TARGET_NAME}

    PRIVATE
        Chicane::Box
)

CH_ADD_COMPILER(${BOX_TARGET_NAME})
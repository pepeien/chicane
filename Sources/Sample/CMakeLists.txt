######################### Files ##########################
file(
    GLOB_RECURSE
    GAME_HEADERS
    CONFIGURE_DEPENDS
        ${CHICANE_SAMPLE_INCLUDES_DIR}/Shooter/*.hpp
        ${CHICANE_SAMPLE_INCLUDES_DIR}/Export.hpp
)
file(
    GLOB_RECURSE
    GAME_SOURCES
    CONFIGURE_DEPENDS
        ${CHICANE_SAMPLE_SOURCES_DIR}/Shooter/*.cpp
        ${CHICANE_SAMPLE_SOURCES_DIR}/Export.cpp
)

set(HOST_SOURCES
    ${CHICANE_SAMPLE_INCLUDES_DIR}/Application.hpp
    ${CHICANE_SAMPLE_INCLUDES_DIR}/Main.hpp
    ${CHICANE_SAMPLE_SOURCES_DIR}/Application.cpp
    ${CHICANE_SAMPLE_SOURCES_DIR}/Main.cpp
)

##################### Game module #######################
set(GAME ${CHICANE_SAMPLE_GAME_TARGET_NAME})

add_library(${GAME} SHARED ${GAME_HEADERS} ${GAME_SOURCES})

add_library("${NAME}::Sample" ALIAS ${GAME})

set_target_properties(
    ${GAME}
    PROPERTIES
        OUTPUT_NAME                 "${NAME}Sample"
        POSITION_INDEPENDENT_CODE   TRUE
        VS_DEBUGGER_WORKING_DIRECTORY "${CHICANE_BUILD_DIR}"
        CH_REFLECT_HEADERS          "${GAME_HEADERS}"
        CH_BASE_HEADER_DIR          "${CHICANE_SOURCES_DIR}"
        CH_SOURCE_HEADER_DIR        "${CHICANE_SAMPLE_INCLUDES_DIR}"
)

target_compile_definitions(${GAME} PRIVATE BUILD_EXPORT BUILD_SHARED)

target_include_directories(
    ${GAME}
    PUBLIC
        $<BUILD_INTERFACE:${CHICANE_BUILD_INTERFACE_DIR}>
        $<INSTALL_INTERFACE:${CHICANE_INSTALL_INTERFACE_DIR}>
    PRIVATE
        ${CHICANE_SOURCES_DIR}
)

target_link_libraries(${GAME} PUBLIC Chicane::Runtime)

CH_ADD_COMPILER(${GAME})
CH_GENERATE_REFLECTION(${GAME})

##################### Host executable #######################
set(HOST ${CHICANE_SAMPLE_TARGET_NAME})

add_executable(${HOST} ${HOST_SOURCES})

set_target_properties(
    ${HOST}
    PROPERTIES
        OUTPUT_NAME                   "${NAME}Sample"
        VS_DEBUGGER_WORKING_DIRECTORY "${CHICANE_BUILD_DIR}"
)

target_include_directories(${HOST} PRIVATE ${CHICANE_SOURCES_DIR})
target_link_libraries(${HOST} PRIVATE Chicane::Sample)

CH_ADD_COMPILER(${HOST})

######################### Dependencies #########################
add_dependencies(${GAME} ${CHICANE_SAMPLE_ASSETS_TARGET_NAME})
add_dependencies(${HOST} ${GAME})

if (TARGET ${CHICANE_EDITOR_TARGET_NAME})
    add_dependencies(${CHICANE_EDITOR_TARGET_NAME} ${GAME})
endif()

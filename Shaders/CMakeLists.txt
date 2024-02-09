##############  Set source files  #######################
file(
    GLOB_RECURSE
    SOURCES

    "${CMAKE_CURRENT_SOURCE_DIR}/*.frag"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.vert"
)

##############  Copy files  #######################
find_program(GLSL_VALIDATOR glslangValidator REQUIRED)

foreach(GLSL ${SOURCES})
    get_filename_component(FILE_DIR  ${GLSL} DIRECTORY)
    get_filename_component(FILE_NAME ${GLSL} NAME)

    set(SPIRV_DIR "${FILE_DIR}")
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" SPIRV_DIR ${SPIRV_DIR})

    set(SPIRV_DIR "${BUILD_DIR}/Shaders/${SPIRV_DIR}")
    string(REPLACE "//" "/" SPIRV_DIR ${SPIRV_DIR})

    file(MAKE_DIRECTORY "${SPIRV_DIR}")

    set(SPIRV "${SPIRV_DIR}/${FILE_NAME}.spv")
    string(REPLACE "//" "/" SPIRV ${SPIRV})

    execute_process(
        COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV}
        OUTPUT_VARIABLE ${SPIRV}
    )

    list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

add_custom_target(
    ${SHADERS_TARGET_NAME}
    DEPENDS

    ${SPIRV_BINARY_FILES}
)
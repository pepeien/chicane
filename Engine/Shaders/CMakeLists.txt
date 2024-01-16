##############  Set values  #######################
set(TARGET_NAME "${ENGINE_SHADERS_TARGET_NAME}")
set(SOURCES_DIR "${ENGINE_SHADERS_SOURCE_DIR}")

##############  Set source files  #######################
file(
    GLOB_RECURSE
    SOURCES

    "${SOURCES_DIR}/*.frag"
    "${SOURCES_DIR}/*.vert"
)

##############  Copy files  #######################
find_program(GLSL_VALIDATOR glslangValidator REQUIRED)

foreach(GLSL ${SOURCES})
    get_filename_component(FILE_DIR  ${GLSL} DIRECTORY)
    get_filename_component(FILE_NAME ${GLSL} NAME)

    set(SPIRV_DIR "${FILE_DIR}")
    string(REPLACE "${SOURCES_DIR}" "" SPIRV_DIR ${SPIRV_DIR})

    set(SPIRV_DIR "${ENGINE_SHADER_BUILD_DIR}/${SPIRV_DIR}")
    string(REPLACE "//" "/" SPIRV_DIR ${SPIRV_DIR})

    file(MAKE_DIRECTORY "${SPIRV_DIR}")

    set(SPIRV "${SPIRV_DIR}/${FILE_NAME}.spv")
    string(REPLACE "//" "/" SPIRV ${SPIRV})

    add_custom_command(
        OUTPUT ${SPIRV}
        COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV}
        DEPENDS ${GLSL}
    )

    list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

add_custom_target(
    ${TARGET_NAME}
    DEPENDS

    ${SPIRV_BINARY_FILES}
)
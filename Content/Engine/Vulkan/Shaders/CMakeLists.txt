add_custom_target(${RUNTIME_CONTENT_VULKAN_SHADERS_TARGET_NAME})

##############  Set source files  #######################
file(
    GLOB_RECURSE

    SOURCES
        "${RUNTIME_CONTENT_VULKAN_SHADERS_SOURCE_DIR}/*.frag"
        "${RUNTIME_CONTENT_VULKAN_SHADERS_SOURCE_DIR}/*.vert"
)

##############  Copy files  #######################
find_program(GLSL_VALIDATOR glslangValidator REQUIRED)

foreach(GLSL ${SOURCES})
    get_filename_component(FILE_DIR  ${GLSL} DIRECTORY)
    get_filename_component(FILE_NAME ${GLSL} NAME)

    set(SPIRV_DIR "${FILE_DIR}")
    string(REPLACE "${CONTENT_DIR}" "" SPIRV_DIR ${SPIRV_DIR})

    set(SPIRV_DIR "${BUILD_DIR}/Content/${SPIRV_DIR}")
    string(REPLACE "//" "/" SPIRV_DIR ${SPIRV_DIR})

    file(MAKE_DIRECTORY "${SPIRV_DIR}")

    set(SPIRV "${SPIRV_DIR}/${FILE_NAME}.spv")
    string(REPLACE "//" "/" SPIRV ${SPIRV})

    add_custom_command(
        TARGET
            ${RUNTIME_CONTENT_VULKAN_SHADERS_TARGET_NAME}
        POST_BUILD
        COMMAND
            ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV}
    )
endforeach(GLSL)
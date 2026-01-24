function(CH_ADD_COMPILER TARGET_NAME)
    if (CMAKE_GENERATOR STREQUAL "MinGW Makefiles")
        if (NOT MINGW_PATH) 
              message(FATAL_ERROR "MINGW_PATH not set in .env.cmake")
        endif()

        set(CMAKE_C_COMPILER   ${MINGW_PATH}/bin/gcc.exe)
        set(CMAKE_CXX_COMPILER ${MINGW_PATH}/bin/g++.exe)

        if (WIN32)
            target_include_directories(
                ${TARGET_NAME}

                PRIVATE
                    ${MINGW_PATH}/include
            )

            target_link_directories(
                ${TARGET_NAME}

                PRIVATE
                    ${MINGW_PATH}/lib
            )
        endif()
    endif()
endfunction()

function(CH_COPY_FILES TARGET_NAME SOURCE_PATH OUTPUT_PATH FILTER_VALUE)
    file(
        GLOB_RECURSE

        SOURCES
            "${SOURCE_PATH}/*"
    )

    if (FILTER_VALUE)
        list(
            FILTER
                SOURCES
            EXCLUDE
                REGEX
                    ${FILTER_VALUE}
        )
    endif()

    foreach(ITEM ${SOURCES})
        get_filename_component(FILE_DIR  ${ITEM} DIRECTORY)
        get_filename_component(FILE_NAME ${ITEM} NAME)

        set(ITEM_DIR "${FILE_DIR}")
        string(REPLACE "${SOURCE_PATH}" "" ITEM_DIR ${ITEM_DIR})

        set(ITEM_DIR "${OUTPUT_PATH}/${ITEM_DIR}")
        string(REPLACE "//" "/" ITEM_DIR ${ITEM_DIR})

        file(MAKE_DIRECTORY "${ITEM_DIR}")

        set(ITEM_FILE "${ITEM_DIR}/${FILE_NAME}")
        string(REPLACE "//" "/" ITEM_FILE ${ITEM_FILE})

        add_custom_command(
            TARGET
                ${TARGET_NAME}
            POST_BUILD
            COMMAND
                ${CMAKE_COMMAND} -E copy
                    ${ITEM}
                    ${ITEM_FILE}
        )
    endforeach(ITEM)
endfunction()

function(
    _CH_COMPILE_SHADERS
    TARGET_NAME
    SOURCE_PATH
    OUTPUT_PATH
    FRAG_SHADER_EXTENSION
    VERT_SHADER_EXTENSION
    SHADER_VERSION
)
    file(
        GLOB_RECURSE

        VULKAN_SOURCES
            "${SOURCE_PATH}/*.${FRAG_SHADER_EXTENSION}"
            "${SOURCE_PATH}/*.${VERT_SHADER_EXTENSION}"
    )

    foreach(GLSL ${VULKAN_SOURCES})
        get_filename_component(FILE_DIR       ${GLSL} DIRECTORY)
        get_filename_component(FILE_NAME      ${GLSL} NAME)
        get_filename_component(FILE_EXTENSION ${GLSL} EXT)

        set(SPIRV_DIR "${FILE_DIR}")
        string(REPLACE "${SOURCE_PATH}" "" SPIRV_DIR ${SPIRV_DIR})

        set(SPIRV_DIR "${OUTPUT_PATH}/${SPIRV_DIR}")
        string(REPLACE "//" "/" SPIRV_DIR ${SPIRV_DIR})

        file(MAKE_DIRECTORY "${SPIRV_DIR}")

        set(SPIRV "${SPIRV_DIR}/${FILE_NAME}")
        string(REPLACE "//" "/" SPIRV ${SPIRV})

        if(FILE_EXTENSION STREQUAL ".${FRAG_SHADER_EXTENSION}")
            message("${GLSL_VALIDATOR} -S frag -${SHADER_VERSION} ${GLSL} -o ${SPIRV}")
            add_custom_command(
                TARGET
                    ${TARGET_NAME}
                POST_BUILD
                COMMAND
                    ${GLSL_VALIDATOR} -S frag -${SHADER_VERSION} ${GLSL} -o ${SPIRV}
            )
        elseif (FILE_EXTENSION STREQUAL ".${VERT_SHADER_EXTENSION}")
            add_custom_command(
                TARGET
                    ${TARGET_NAME}
                POST_BUILD
                COMMAND
                    ${GLSL_VALIDATOR} -S vert -${SHADER_VERSION} ${GLSL} -o ${SPIRV}
            )
        endif()
    endforeach(GLSL)
endfunction()

function(CH_COMPILE_SHADERS TARGET_NAME SOURCE_PATH OUTPUT_PATH)
    find_program(GLSL_VALIDATOR glslangValidator REQUIRED)

    _CH_COMPILE_SHADERS(${TARGET_NAME} ${SOURCE_PATH} ${OUTPUT_PATH} "vfrag" "vvert" "V")
    _CH_COMPILE_SHADERS(${TARGET_NAME} ${SOURCE_PATH} ${OUTPUT_PATH} "ofrag" "overt" "G")
endfunction()

function(CH_INSTALL_FILES SOURCES SOURCE_DIR OUTPUT_DIR)
    file(
        GLOB_RECURSE

        FILES
            ${SOURCES}
    )

    foreach(FILE ${FILES})
        get_filename_component(FILE_DIR  ${FILE} DIRECTORY)
        get_filename_component(FILE_NAME ${FILE} NAME)

        set(RELATIVE_DIR "${FILE_DIR}")
        string(REPLACE "${SOURCE_DIR}" "" RELATIVE_DIR ${RELATIVE_DIR})

        set(RESULT_DIR "${OUTPUT_DIR}/${RELATIVE_DIR}")
        string(REPLACE "//" "/" RESULT_DIR ${RESULT_DIR})

        install(
            FILES
                ${FILE}
            DESTINATION
                ${RESULT_DIR}
        )
    endforeach(FILE)
endfunction()
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
    file(GLOB_RECURSE SOURCES "${SOURCE_PATH}/*")

    if (FILTER_VALUE)
        list(FILTER SOURCES EXCLUDE REGEX ${FILTER_VALUE})
    endif()

    set(ALL_OUTPUTS "")

    foreach(ITEM ${SOURCES})
        file(RELATIVE_PATH REL_PATH "${SOURCE_PATH}" "${ITEM}")

        set(ITEM_FILE "${OUTPUT_PATH}/${REL_PATH}")

        get_filename_component(ITEM_DIR "${ITEM_FILE}" DIRECTORY)

        add_custom_command(
            OUTPUT  "${ITEM_FILE}"
            COMMAND ${CMAKE_COMMAND} -E make_directory "${ITEM_DIR}"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${ITEM}" "${ITEM_FILE}"
            DEPENDS "${ITEM}"
            COMMENT "Copying ${REL_PATH}"
        )

        list(APPEND ALL_OUTPUTS "${ITEM_FILE}")
    endforeach()

    add_custom_target(
        "${TARGET_NAME}-Copy" ALL
        DEPENDS ${ALL_OUTPUTS}
    )

    add_dependencies(${TARGET_NAME} "${TARGET_NAME}-Copy")
endfunction()

function(CH_COMPILE_SHADERS TARGET_NAME SOURCE_PATH OUTPUT_PATH)
    cmake_parse_arguments(SHADER "" "" "INCLUDES" ${ARGN})

    find_program(SLANGC slangc HINTS "$ENV{VULKAN_SDK}/Bin" "$ENV{VULKAN_SDK}/bin" REQUIRED)
    find_program(SPIRV_CROSS spirv-cross HINTS "$ENV{VULKAN_SDK}/Bin" "$ENV{VULKAN_SDK}/bin" REQUIRED)
    find_program(GLSL_VALIDATOR glslangValidator HINTS "$ENV{VULKAN_SDK}/Bin" "$ENV{VULKAN_SDK}/bin")

    file(GLOB_RECURSE SLANG_SOURCES "${SOURCE_PATH}/*.slang")
    file(GLOB_RECURSE SLANG_MODULES "${SOURCE_PATH}/*.slang")

    set(INCLUDE_ARGS "")
    list(APPEND INCLUDE_ARGS "-I${SOURCE_PATH}")
    list(APPEND INCLUDE_ARGS "-I${SOURCE_PATH}/Shaders")
    list(APPEND INCLUDE_ARGS "-I${SOURCE_PATH}/Shaders/Common")
    list(APPEND INCLUDE_ARGS "-I${SOURCE_PATH}/Common")
    foreach(INCLUDE_PATH ${SHADER_INCLUDES})
        list(APPEND INCLUDE_ARGS "-I${INCLUDE_PATH}")
        list(APPEND INCLUDE_ARGS "-I${INCLUDE_PATH}/Common")
        file(GLOB_RECURSE EXTRA_MODULES "${INCLUDE_PATH}/*.slang")
        list(APPEND SLANG_MODULES ${EXTRA_MODULES})
    endforeach()

    set(ALL_OUTPUTS "")

    foreach(SLANG ${SLANG_SOURCES})
        get_filename_component(FILE_NAME ${SLANG} NAME)

        if(FILE_NAME MATCHES "\\.vert\\.slang$")
            set(SHADER_STAGE "vertex")
            set(VALIDATOR_STAGE "vert")
        elseif(FILE_NAME MATCHES "\\.frag\\.slang$")
            set(SHADER_STAGE "fragment")
            set(VALIDATOR_STAGE "frag")
        else()
            continue()
        endif()

        get_filename_component(FILE_DIR ${SLANG} DIRECTORY)
        set(REL_DIR "${FILE_DIR}")
        string(REPLACE "${SOURCE_PATH}" "" REL_DIR "${REL_DIR}")
        set(OUT_DIR "${OUTPUT_PATH}/${REL_DIR}")
        string(REPLACE "//" "/" OUT_DIR "${OUT_DIR}")

        string(REPLACE ".slang" "" STAGE_NAME "${FILE_NAME}")
        set(VSPV "${OUT_DIR}/${STAGE_NAME}.vspv")
        set(GLSL "${OUT_DIR}/${STAGE_NAME}.glsl")
        set(TMP_SPV "${OUT_DIR}/${STAGE_NAME}.gl.spv")
        string(REPLACE "//" "/" VSPV "${VSPV}")
        string(REPLACE "//" "/" GLSL "${GLSL}")
        string(REPLACE "//" "/" TMP_SPV "${TMP_SPV}")

        set(VALIDATE_COMMAND ${CMAKE_COMMAND} -E true)
        if(GLSL_VALIDATOR)
            set(VALIDATE_COMMAND ${GLSL_VALIDATOR} -S ${VALIDATOR_STAGE} ${GLSL})
        endif()

        add_custom_command(
            OUTPUT
                "${VSPV}"
                "${GLSL}"
            COMMAND
                ${CMAKE_COMMAND} -E make_directory "${OUT_DIR}"
            COMMAND
                ${SLANGC} ${SLANG} -target spirv -stage ${SHADER_STAGE} -entry main -DCHICANE_VULKAN ${INCLUDE_ARGS} -o "${VSPV}"
            COMMAND
                ${SLANGC} ${SLANG} -target spirv -stage ${SHADER_STAGE} -entry main ${INCLUDE_ARGS} -o "${TMP_SPV}"
            COMMAND
                ${SPIRV_CROSS} "${TMP_SPV}" --version 460 --no-es --output "${GLSL}"
            COMMAND
                ${VALIDATE_COMMAND}
            DEPENDS
                "${SLANG}"
                ${SLANG_MODULES}
            COMMENT
                "Compiling shader ${FILE_NAME}"
            VERBATIM
        )

        list(APPEND ALL_OUTPUTS "${VSPV}" "${GLSL}")
    endforeach()

    if(ALL_OUTPUTS)
        string(MD5 SHADER_HASH "${SOURCE_PATH}")
        add_custom_target(
            "CH_SHADERS_${SHADER_HASH}" ALL
            DEPENDS ${ALL_OUTPUTS}
        )
        add_dependencies(${TARGET_NAME} "CH_SHADERS_${SHADER_HASH}")
    endif()
endfunction()

function(CH_INSTALL_FILES SOURCES SOURCE_DIR OUTPUT_DIR)
    file(
        GLOB_RECURSE
        FILES
            ${SOURCES}
    )

    set(ALL_OUTPUTS "")

    foreach(FILE ${FILES})
        get_filename_component(FILE_DIR  ${FILE} DIRECTORY)
        get_filename_component(FILE_NAME ${FILE} NAME)

        set(RELATIVE_DIR "${FILE_DIR}")
        string(REPLACE "${SOURCE_DIR}" "" RELATIVE_DIR ${RELATIVE_DIR})

        set(RESULT_DIR "${OUTPUT_DIR}/${RELATIVE_DIR}")
        string(REPLACE "//" "/" RESULT_DIR ${RESULT_DIR})

        set(RESULT_FILE "${RESULT_DIR}/${FILE_NAME}")
        string(REPLACE "//" "/" RESULT_FILE ${RESULT_FILE})

        get_property(REGISTERED GLOBAL PROPERTY "CH_INSTALL_REGISTERED_${RESULT_FILE}")
        if (REGISTERED)
            continue()
        endif()
        set_property(GLOBAL PROPERTY "CH_INSTALL_REGISTERED_${RESULT_FILE}" TRUE)

        add_custom_command(
            OUTPUT  "${RESULT_FILE}"
            COMMAND ${CMAKE_COMMAND} -E make_directory "${RESULT_DIR}"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${FILE}" "${RESULT_FILE}"
            DEPENDS "${FILE}"
            COMMENT "Installing ${FILE_NAME}"
        )

        list(APPEND ALL_OUTPUTS "${RESULT_FILE}")
    endforeach()

    if (ALL_OUTPUTS)
        string(MD5 DIR_HASH "${OUTPUT_DIR}")

        add_custom_target(
            "CH_INSTALL_FILES_${DIR_HASH}" ALL
            DEPENDS ${ALL_OUTPUTS}
        )
    endif()
endfunction()
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
        "${TARGET_NAME}-copy" ALL
        DEPENDS ${ALL_OUTPUTS}
    )

    add_dependencies(${TARGET_NAME} "${TARGET_NAME}-copy")
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

        # Skip if already registered
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

function(CH_REFLECT_FILE TARGET_NAME SOURCES BASE_DIR SOURCE_DIR)
    set(OUTPUT_DIR "${CMAKE_BINARY_DIR}/Reflected")
    set(GENERATED_FILES "")
    set(ALL_INPUT_FILES "")

    foreach(FILE ${SOURCES})
        get_filename_component(FILE_DIR  "${FILE}" DIRECTORY)
        get_filename_component(FILE_BASE "${FILE}" NAME_WE)
        get_filename_component(FILE_EXT  "${FILE}" EXT)

        # Mirror source structure under OUTPUT_DIR
        file(RELATIVE_PATH RELATIVE_DIR "${BASE_DIR}" "${FILE_DIR}")

        if(RELATIVE_DIR STREQUAL "" OR RELATIVE_DIR STREQUAL ".")
            set(RESULT_DIR "${OUTPUT_DIR}")
        else()
            set(RESULT_DIR "${OUTPUT_DIR}/${RELATIVE_DIR}")
        endif()

        # e.g. Reflected/Chicane/Core/Telemetry.reflected.hpp
        set(RESULT_FILE "${RESULT_DIR}/${FILE_BASE}.reflected${FILE_EXT}")

        file(MAKE_DIRECTORY "${RESULT_DIR}")
        list(APPEND GENERATED_FILES "${RESULT_FILE}")
        list(APPEND ALL_INPUT_FILES "${FILE}")
    endforeach()

    add_custom_command(
        OUTPUT   ${GENERATED_FILES}
        COMMAND  dotnet run
                 --project "${PROGRAMS_SOURCE_DIR}/Reflector/Reflector.csproj"
                 -i ${ALL_INPUT_FILES}
                 -b "${BASE_DIR}"
                 -s "${SOURCE_DIR}"   
                 -o "${OUTPUT_DIR}"
        DEPENDS  ${SOURCES}
        COMMENT  ""
        VERBATIM
    )

    set(REFLECT_TARGET "Reflect_${TARGET_NAME}")
    add_custom_target(${REFLECT_TARGET} DEPENDS ${GENERATED_FILES})
    add_dependencies(${TARGET_NAME} ${REFLECT_TARGET})
    target_sources(${TARGET_NAME} PRIVATE ${GENERATED_FILES})
    target_include_directories(${TARGET_NAME} PRIVATE "${OUTPUT_DIR}")
endfunction()
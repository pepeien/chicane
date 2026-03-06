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

function(_CH_COLLECT_REFLECT_HEADERS TARGET OUT_HEADERS VISITED)
    if(${TARGET} IN_LIST ${VISITED})
        return()
    endif()

    list(APPEND ${VISITED} ${TARGET})

    get_target_property(HEADERS ${TARGET} CH_REFLECT_HEADERS)

    if(HEADERS AND NOT HEADERS STREQUAL "HEADERS-NOTFOUND")
        list(APPEND ${OUT_HEADERS} ${HEADERS})
    endif()

    get_target_property(LINKED_LIBS ${TARGET} LINK_LIBRARIES)

    if(LINKED_LIBS AND NOT LINKED_LIBS STREQUAL "LINKED_LIBS-NOTFOUND")
        foreach(LIB ${LINKED_LIBS})
            if(LIB MATCHES "^\\$<" OR NOT TARGET ${LIB})
                continue()
            endif()

            _CH_COLLECT_REFLECT_HEADERS(${LIB} ${OUT_HEADERS} ${VISITED})
        endforeach()
    endif()

    set(${OUT_HEADERS} ${${OUT_HEADERS}} PARENT_SCOPE)
    set(${VISITED}     ${${VISITED}}     PARENT_SCOPE)
endfunction()

function(_CH_GET_ALL_TARGETS OUT DIR)
    get_property(TGTS DIRECTORY "${DIR}" PROPERTY BUILDSYSTEM_TARGETS)

    list(APPEND ${OUT} ${TGTS})

    get_property(SUBDIRS DIRECTORY "${DIR}" PROPERTY SUBDIRECTORIES)

    foreach(SUBDIR IN LISTS SUBDIRS)
        _CH_GET_ALL_TARGETS(${OUT} "${SUBDIR}")

        set(${OUT} ${${OUT}} PARENT_SCOPE)
    endforeach()

    set(${OUT} ${${OUT}} PARENT_SCOPE)
endfunction()

function(CH_GENERATE_REFLECTION)
    set(ALL_TARGETS)
    _CH_GET_ALL_TARGETS(ALL_TARGETS "${CMAKE_CURRENT_LIST_DIR}")

    foreach(T ${ALL_TARGETS})
        get_target_property(HEADERS    ${T} CH_REFLECT_HEADERS)
        get_target_property(BASE_DIR   ${T} CH_BASE_HEADER_DIR)
        get_target_property(SOURCE_DIR ${T} CH_SOURCE_HEADER_DIR)

        if(NOT HEADERS OR HEADERS STREQUAL "HEADERS-NOTFOUND")
            continue()
        endif()

        if(NOT BASE_DIR OR BASE_DIR STREQUAL "BASE_DIR-NOTFOUND")
            continue()
        endif()

        if(NOT SOURCE_DIR OR SOURCE_DIR STREQUAL "SOURCE_DIR-NOTFOUND")
            continue()
        endif()

        set(OUTPUT_DIR "${CMAKE_BINARY_DIR}/Reflected")
        set(GENERATED_FILES)

        set(VISITED)
        set(ALL_INPUT_FILES)

        _CH_COLLECT_REFLECT_HEADERS(${T} ALL_INPUT_FILES VISITED)

        list(REMOVE_DUPLICATES ALL_INPUT_FILES)

        set(TARGET_GENERATED_FILES)

        foreach(FILE ${HEADERS})
            get_filename_component(FILE_DIR  "${FILE}" DIRECTORY)
            get_filename_component(FILE_BASE "${FILE}" NAME_WE)
            get_filename_component(FILE_EXT  "${FILE}" EXT)

            file(RELATIVE_PATH RELATIVE_DIR "${BASE_DIR}" "${FILE_DIR}")

            set(RESULT_DIR "${OUTPUT_DIR}/${RELATIVE_DIR}")
            string(REPLACE "//" "/" RESULT_DIR ${RESULT_DIR})

            list(APPEND TARGET_GENERATED_FILES "${RESULT_DIR}/${FILE_BASE}.reflected${FILE_EXT}")
        endforeach()

        if(NOT TARGET_GENERATED_FILES)
            continue()
        endif()

        add_custom_command(
            OUTPUT  ${TARGET_GENERATED_FILES}
            COMMAND dotnet run
                    --project "${PROGRAMS_SOURCE_DIR}/Reflector/Reflector.csproj"
                    -i ${ALL_INPUT_FILES}   # Own headers + linked lib headers (type resolution)
                    -b "${BASE_DIR}"
                    -s "${SOURCE_DIR}"      # C# only emits files under this dir
                    -o "${OUTPUT_DIR}"
            DEPENDS ${ALL_INPUT_FILES}      # Rebuild if any header changes
            COMMENT ""
            VERBATIM
        )

        foreach(GEN_FILE ${TARGET_GENERATED_FILES})
            set_source_files_properties(
                "${GEN_FILE}"
                DIRECTORY "${CMAKE_SOURCE_DIR}"
                PROPERTIES GENERATED TRUE
            )
        endforeach()

        set(REFLECT_TARGET "Reflect_${T}")

        add_custom_target(${REFLECT_TARGET} ALL
            DEPENDS ${TARGET_GENERATED_FILES}
        )

        add_dependencies(${T} ${REFLECT_TARGET})
        target_include_directories(${T} PRIVATE "${OUTPUT_DIR}")

        list(APPEND GENERATED_FILES ${TARGET_GENERATED_FILES})
    endforeach()
endfunction()
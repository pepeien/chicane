######################### Depedencies #########################
find_program(DOTNET_EXECUTABLE dotnet REQUIRED)

######################### Files #########################
file(
    GLOB_RECURSE
    SOURCES
    CONFIGURE_DEPENDS
    ${CHICANE_PROGRAMS_REFLECTOR_SOURCES_DIR}/*.cs
) 

######################### Setup #########################
if(WIN32)
    set(REFLECTOR_RID "win-x64")
else()
    set(REFLECTOR_RID "linux-x64")
endif()

add_custom_command(
    OUTPUT
        "${CHICANE_PROGRAMS_REFLECTOR_BINARY}"
    COMMAND
        "${DOTNET_EXECUTABLE}" publish "${CHICANE_PROGRAMS_REFLECTOR_SOURCES_DIR}/Reflector.csproj"
        -r ${REFLECTOR_RID}
        -c Release
        --output "${CHICANE_PROGRAMS_REFLECTOR_BINARY_DIR}"
        --nologo
        --verbosity quiet
    DEPENDS
        ${SOURCES}
    COMMENT
        ""
    VERBATIM
)

add_custom_target(
    ${CHICANE_PROGRAMS_REFLECTOR_TARGET_NAME}

    DEPENDS
        "${CHICANE_PROGRAMS_REFLECTOR_BINARY}"
)

######################### Functions #########################
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
        get_target_property(SOURCES_DIR ${T} CH_SOURCE_HEADER_DIR)

        if(NOT HEADERS OR HEADERS STREQUAL "HEADERS-NOTFOUND")
            continue()
        endif()

        if(NOT BASE_DIR OR BASE_DIR STREQUAL "BASE_DIR-NOTFOUND")
            continue()
        endif()

        if(NOT SOURCES_DIR OR SOURCES_DIR STREQUAL "SOURCES_DIR-NOTFOUND")
            continue()
        endif()

        set(OUTPUT_DIR "${CMAKE_BINARY_DIR}/Reflected")
        file(MAKE_DIRECTORY "${OUTPUT_DIR}")

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
            OUTPUT 
                ${TARGET_GENERATED_FILES}
            COMMAND
                "${CHICANE_PROGRAMS_REFLECTOR_BINARY}"
                -i ${ALL_INPUT_FILES}
                -b "${BASE_DIR}"
                -s "${SOURCES_DIR}"
                -o "${OUTPUT_DIR}"
            BYPRODUCTS
                ${TARGET_GENERATED_FILES}
            DEPENDS
                ${ALL_INPUT_FILES} ${CHICANE_PROGRAMS_REFLECTOR_TARGET_NAME}
            COMMENT
                ""
            VERBATIM
        )

        foreach(GEN_FILE ${TARGET_GENERATED_FILES})
            set_source_files_properties(
                "${GEN_FILE}"
                DIRECTORY
                    "${CMAKE_SOURCE_DIR}"
                PROPERTIES
                    GENERATED
                        TRUE
            )
        endforeach()

        set(REFLECT_TARGET "${T}-Reflect")

        add_custom_target(
            ${REFLECT_TARGET}
            ALL
            DEPENDS
                ${TARGET_GENERATED_FILES}
        )

        add_dependencies(${REFLECT_TARGET} ${CHICANE_PROGRAMS_REFLECTOR_TARGET_NAME}) 
        add_dependencies(${T} ${REFLECT_TARGET})

        target_include_directories(${T} PRIVATE "${OUTPUT_DIR}")

        list(APPEND GENERATED_FILES ${TARGET_GENERATED_FILES})
    endforeach()
endfunction()
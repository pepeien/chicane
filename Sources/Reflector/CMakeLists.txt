######################### Depedencies #########################
find_program(DOTNET_EXECUTABLE dotnet REQUIRED)

######################### Values #########################
set(CHICANE_REFLECTOR_TARGET_NAME "${NAME}-Reflector"                CACHE INTERNAL "")
set(CHICANE_REFLECTOR_SOURCES_DIR "${CHICANE_SOURCES_DIR}/Reflector" CACHE INTERNAL "")
set(CHICANE_REFLECTOR_BINARY_DIR  "${CHICANE_BUILD_DIR}"             CACHE INTERNAL "")

if(WIN32)
    set(CHICANE_REFLECTOR_BINARY "${CHICANE_BUILD_DIR}/${NAME}Reflector.exe" CACHE INTERNAL "")
else()
    set(CHICANE_REFLECTOR_BINARY "${CHICANE_BUILD_DIR}/${NAME}Reflector"     CACHE INTERNAL "")
endif()

######################### Files #########################
file(
    GLOB_RECURSE
    SOURCES
    CONFIGURE_DEPENDS
    ${CHICANE_REFLECTOR_SOURCES_DIR}/*.cs
) 

######################### Setup #########################
if(WIN32)
    set(REFLECTOR_RID "win-x64")
else()
    set(REFLECTOR_RID "linux-x64")
endif()

######################### Functions #########################
add_custom_command(
    OUTPUT
        "${CHICANE_REFLECTOR_BINARY}"
    COMMAND
        "${DOTNET_EXECUTABLE}" publish "${CHICANE_REFLECTOR_SOURCES_DIR}/Reflector.csproj"
        -r ${REFLECTOR_RID}
        -c Release
        --output "${CHICANE_REFLECTOR_BINARY_DIR}"
        --nologo
        --verbosity quiet
    DEPENDS
        ${SOURCES}
    COMMENT
        ""
    VERBATIM
)

add_custom_target(
    ${CHICANE_REFLECTOR_TARGET_NAME}

    DEPENDS
        "${CHICANE_REFLECTOR_BINARY}"
)

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
    get_target_property(IFACE_LIBS  ${TARGET} INTERFACE_LINK_LIBRARIES)

    set(ALL_LIBS "")

    if(LINKED_LIBS AND NOT LINKED_LIBS STREQUAL "LINKED_LIBS-NOTFOUND")
        list(APPEND ALL_LIBS ${LINKED_LIBS})
    endif()

    if(IFACE_LIBS AND NOT IFACE_LIBS STREQUAL "IFACE_LIBS-NOTFOUND")
        list(APPEND ALL_LIBS ${IFACE_LIBS})
    endif()

    if(ALL_LIBS)
        list(REMOVE_DUPLICATES ALL_LIBS)

        foreach(LIB ${ALL_LIBS})
            if(LIB MATCHES "^\\$<" OR NOT TARGET ${LIB})
                continue()
            endif()

            _CH_COLLECT_REFLECT_HEADERS(${LIB} ${OUT_HEADERS} ${VISITED})
        endforeach()
    endif()

    set(${OUT_HEADERS} ${${OUT_HEADERS}} PARENT_SCOPE)
    set(${VISITED}     ${${VISITED}}     PARENT_SCOPE)
endfunction()

function(_CH_GET_ALL_TARGETS OUT TARGET)
    if(NOT TARGET ${TARGET})
        return()
    endif()

    get_target_property(_ALIASED ${TARGET} ALIASED_TARGET)

    if(_ALIASED)
        set(TARGET ${_ALIASED})
    endif()

    if(${TARGET} IN_LIST ${OUT})
        return()
    endif()

    list(APPEND ${OUT} ${TARGET})

    get_target_property(LINKED_LIBS ${TARGET} LINK_LIBRARIES)
    get_target_property(IFACE_LIBS  ${TARGET} INTERFACE_LINK_LIBRARIES)

    set(ALL_LIBS "")

    if(LINKED_LIBS AND NOT LINKED_LIBS STREQUAL "LINKED_LIBS-NOTFOUND")
        list(APPEND ALL_LIBS ${LINKED_LIBS})
    endif()

    if(IFACE_LIBS AND NOT IFACE_LIBS STREQUAL "IFACE_LIBS-NOTFOUND")
        list(APPEND ALL_LIBS ${IFACE_LIBS})
    endif()

    list(REMOVE_DUPLICATES ALL_LIBS)

    foreach(LIB ${ALL_LIBS})
        if(LIB MATCHES "^\\$<" OR NOT TARGET ${LIB})
            continue()
        endif()

        set(_SUB_OUT ${${OUT}})

        _CH_GET_ALL_TARGETS(_SUB_OUT ${LIB})

        foreach(_T ${_SUB_OUT})
            if(NOT _T IN_LIST ${OUT})
                list(APPEND ${OUT} ${_T})
            endif()
        endforeach()
    endforeach()

    set(${OUT} ${${OUT}} PARENT_SCOPE)
endfunction()

function(CH_GENERATE_REFLECTION TARGET)
    get_target_property(HEADERS      ${TARGET} CH_REFLECT_HEADERS)
    get_target_property(BASE_DIR     ${TARGET} CH_BASE_HEADER_DIR)
    get_target_property(SOURCES_DIR  ${TARGET} CH_SOURCE_HEADER_DIR)

    if(NOT HEADERS OR HEADERS STREQUAL "HEADERS-NOTFOUND")
        return()
    endif()

    if(NOT BASE_DIR OR BASE_DIR STREQUAL "BASE_DIR-NOTFOUND")
        return()
    endif()

    if(NOT SOURCES_DIR OR SOURCES_DIR STREQUAL "SOURCES_DIR-NOTFOUND")
        return()
    endif()

    set(TARGETS)
    _CH_GET_ALL_TARGETS(TARGETS ${TARGET})

    set(INPUT_FILES)
    set(LOOK_UP_DIRS)
    foreach(T ${TARGETS})
        get_target_property(INCLUDE_DIRS ${T} INTERFACE_INCLUDE_DIRECTORIES)

        if(INCLUDE_DIRS AND NOT INCLUDE_DIRS STREQUAL "INCLUDE_DIRS-NOTFOUND")
            foreach(DIR ${INCLUDE_DIRS})
                if(DIR MATCHES "^\\$<BUILD_INTERFACE:(.*)>$")
                    list(APPEND LOOK_UP_DIRS "${CMAKE_MATCH_1}")
                elseif(NOT DIR MATCHES "^\\$<")
                    list(APPEND LOOK_UP_DIRS "${DIR}")
                endif()
            endforeach()
        endif()
        list(REMOVE_DUPLICATES LOOK_UP_DIRS)

        set(VISITED)
        _CH_COLLECT_REFLECT_HEADERS(${T} INPUT_FILES VISITED)
    endforeach()

    list(REMOVE_DUPLICATES INPUT_FILES)
    set(_ABS_INPUT_FILES)
    foreach(FILE ${INPUT_FILES})
        get_filename_component(_ABS_FILE "${FILE}" ABSOLUTE)

        list(APPEND _ABS_INPUT_FILES "${_ABS_FILE}")
    endforeach()
    set(INPUT_FILES ${_ABS_INPUT_FILES})

    set(GENERATED_FILES)

    foreach(FILE ${HEADERS})
        get_filename_component(FILE_DIR  "${FILE}" DIRECTORY)
        get_filename_component(FILE_BASE "${FILE}" NAME_WE)
        get_filename_component(FILE_EXT  "${FILE}" EXT)

        file(RELATIVE_PATH RELATIVE_DIR "${BASE_DIR}" "${FILE_DIR}")

        set(RESULT_DIR "${OUTPUT_DIR}/${RELATIVE_DIR}")
        string(REPLACE "//" "/" RESULT_DIR ${RESULT_DIR})

        list(APPEND GENERATED_FILES "${RESULT_DIR}/${FILE_BASE}.reflected${FILE_EXT}")
    endforeach()

    if(NOT GENERATED_FILES)
        return()
    endif()

    set(OUTPUT_DIR "${CMAKE_BINARY_DIR}/Reflected")
    file(MAKE_DIRECTORY "${OUTPUT_DIR}")

    set(STAMP_FILE "${CMAKE_CURRENT_BINARY_DIR}/${TARGET}.reflect.stamp")

    add_custom_command(
        OUTPUT
            ${STAMP_FILE}
        COMMAND
            "${CHICANE_REFLECTOR_BINARY}"
            -i ${INPUT_FILES}
            -b "${BASE_DIR}"
            -s "${SOURCES_DIR}"
            -l ${LOOK_UP_DIRS}
            -o "${OUTPUT_DIR}"
        COMMAND
            ${CMAKE_COMMAND} -E touch ${STAMP_FILE}
        BYPRODUCTS
            ${GENERATED_FILES}
        DEPENDS
            ${INPUT_FILES} ${CHICANE_REFLECTOR_TARGET_NAME}
        COMMENT
            ""
        VERBATIM
    )

    foreach(GEN_FILE ${GENERATED_FILES})
        set_source_files_properties(
            "${GEN_FILE}"
            DIRECTORY
                "${CMAKE_SOURCE_DIR}"
            PROPERTIES
                GENERATED
                    TRUE
        )
    endforeach()

    set(REFLECT_TARGET "${TARGET}-Reflect")

    add_custom_target(
        ${REFLECT_TARGET}
        ALL
        DEPENDS
            ${STAMP_FILE}
    )

    add_dependencies(${REFLECT_TARGET} ${CHICANE_REFLECTOR_TARGET_NAME})
    add_dependencies(${TARGET} ${REFLECT_TARGET})

    target_include_directories(${TARGET} PRIVATE "${OUTPUT_DIR}")
endfunction()
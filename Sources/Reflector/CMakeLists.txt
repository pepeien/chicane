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

function(_CH_RESOLVE_TARGET OUT TARGET)
    set(_TARGET ${TARGET})

    if(TARGET ${_TARGET})
        get_target_property(_ALIASED ${_TARGET} ALIASED_TARGET)

        if(_ALIASED)
            set(_TARGET ${_ALIASED})
        endif()
    endif()

    set(${OUT} ${_TARGET} PARENT_SCOPE)
endfunction()

function(_CH_GET_LINKED_TARGETS OUT TARGET)
    if(NOT TARGET ${TARGET})
        return()
    endif()

    _CH_RESOLVE_TARGET(TARGET ${TARGET})

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
        _CH_GET_LINKED_TARGETS(_SUB_OUT ${LIB})

        foreach(_T ${_SUB_OUT})
            if(NOT _T IN_LIST ${OUT})
                list(APPEND ${OUT} ${_T})
            endif()
        endforeach()
    endforeach()

    set(${OUT} ${${OUT}} PARENT_SCOPE)
endfunction()

function(_CH_GET_REFLECT_DEPENDENCIES OUT TARGET)
    set(_LINKED)
    _CH_GET_LINKED_TARGETS(_LINKED ${TARGET})

    set(_RESULT "")

    foreach(_LIB ${_LINKED})
        if(_LIB STREQUAL "${TARGET}")
            continue()
        endif()

        _CH_RESOLVE_TARGET(_LIB ${_LIB})

        get_target_property(_HEADERS ${_LIB} CH_REFLECT_HEADERS)

        if(_HEADERS AND NOT _HEADERS STREQUAL "HEADERS-NOTFOUND")
            list(APPEND _RESULT ${_LIB})
        endif()
    endforeach()

    list(REMOVE_DUPLICATES _RESULT)
    set(${OUT} ${_RESULT} PARENT_SCOPE)
endfunction()

function(_CH_COLLECT_LOOK_UP_DIRS OUT TARGET)
    set(_LINKED)
    _CH_GET_LINKED_TARGETS(_LINKED ${TARGET})

    set(_DIRS
        "${CHICANE_INCLUDES_DIR}"
        "${CHICANE_SOURCES_DIR}"
    )

    foreach(_T ${_LINKED})
        get_target_property(INCLUDE_DIRS ${_T} INTERFACE_INCLUDE_DIRECTORIES)

        if(INCLUDE_DIRS AND NOT INCLUDE_DIRS STREQUAL "INCLUDE_DIRS-NOTFOUND")
            foreach(DIR ${INCLUDE_DIRS})
                if(DIR MATCHES "^\\$<BUILD_INTERFACE:(.*)>$")
                    list(APPEND _DIRS "${CMAKE_MATCH_1}")
                elseif(NOT DIR MATCHES "^\\$<")
                    list(APPEND _DIRS "${DIR}")
                endif()
            endforeach()
        endif()
    endforeach()

    list(REMOVE_DUPLICATES _DIRS)
    set(${OUT} ${_DIRS} PARENT_SCOPE)
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

    set(OUTPUT_DIR "${CMAKE_BINARY_DIR}/Reflected")
    set(TYPES_DIR  "${OUTPUT_DIR}/Types")
    file(MAKE_DIRECTORY "${OUTPUT_DIR}")
    file(MAKE_DIRECTORY "${TYPES_DIR}")

    set(INPUT_FILES)
    foreach(FILE ${HEADERS})
        get_filename_component(_ABS_FILE "${FILE}" ABSOLUTE)
        list(APPEND INPUT_FILES "${_ABS_FILE}")
    endforeach()
    list(REMOVE_DUPLICATES INPUT_FILES)

    set(LOOK_UP_DIRS)
    _CH_COLLECT_LOOK_UP_DIRS(LOOK_UP_DIRS ${TARGET})

    if(NOT BASE_DIR IN_LIST LOOK_UP_DIRS)
        list(APPEND LOOK_UP_DIRS "${BASE_DIR}")
    endif()
    if(NOT SOURCES_DIR IN_LIST LOOK_UP_DIRS)
        list(APPEND LOOK_UP_DIRS "${SOURCES_DIR}")
    endif()
    list(REMOVE_DUPLICATES LOOK_UP_DIRS)

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

    set(REFLECT_DEPS)
    _CH_GET_REFLECT_DEPENDENCIES(REFLECT_DEPS ${TARGET})

    set(DEP_TYPE_FILES)
    foreach(DEP ${REFLECT_DEPS})
        list(APPEND DEP_TYPE_FILES "${TYPES_DIR}/${DEP}.types")
    endforeach()

    set(TYPES_FILE "${TYPES_DIR}/${TARGET}.types")
    if(NOT EXISTS "${TYPES_FILE}")
        file(WRITE "${TYPES_FILE}" "")
    endif()

    set(INPUTS_FILE "${CMAKE_CURRENT_BINARY_DIR}/${TARGET}.reflect.inputs")
    set(_INPUTS_CONTENT "")
    foreach(_INPUT ${INPUT_FILES})
        string(APPEND _INPUTS_CONTENT "${_INPUT}\n")
    endforeach()
    file(WRITE "${INPUTS_FILE}" "${_INPUTS_CONTENT}")

    set(INCLUDES_FILE "${CMAKE_CURRENT_BINARY_DIR}/${TARGET}.reflect.includes")
    set(_INCLUDES_CONTENT "")
    foreach(_DIR ${LOOK_UP_DIRS})
        string(APPEND _INCLUDES_CONTENT "${_DIR}\n")
    endforeach()
    file(WRITE "${INCLUDES_FILE}" "${_INCLUDES_CONTENT}")

    set(STAMP_FILE "${CMAKE_CURRENT_BINARY_DIR}/${TARGET}.reflect.stamp")

    add_custom_command(
        OUTPUT
            ${STAMP_FILE}
        COMMAND
            "${CHICANE_REFLECTOR_BINARY}"
            -f "${INPUTS_FILE}"
            -b "${BASE_DIR}"
            -s "${SOURCES_DIR}"
            -L "${INCLUDES_FILE}"
            -o "${OUTPUT_DIR}"
            -t "${TYPES_DIR}"
            -n "${TARGET}"
        COMMAND
            ${CMAKE_COMMAND} -E touch ${STAMP_FILE}
        BYPRODUCTS
            ${GENERATED_FILES}
            ${TYPES_FILE}
        DEPENDS
            ${INPUT_FILES}
            ${INPUTS_FILE}
            ${INCLUDES_FILE}
            ${DEP_TYPE_FILES}
            ${CHICANE_REFLECTOR_TARGET_NAME}
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

    foreach(DEP ${REFLECT_DEPS})
        add_dependencies(${REFLECT_TARGET} "${DEP}-Reflect")
    endforeach()

    add_dependencies(${TARGET} ${REFLECT_TARGET})

    target_include_directories(${TARGET} PRIVATE "${OUTPUT_DIR}")
endfunction()

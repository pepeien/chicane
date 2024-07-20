add_custom_target(${CONTENT_TARGET_NAME})

##############  Set source files  #######################
file(
    GLOB_RECURSE

    SOURCES
        "${CONTENT_SOURCE_DIR}/*"
)

list(
    FILTER
        SOURCES
    EXCLUDE
    REGEX
        ".*CMakeLists.txt$"
)

##############  Copy files  #######################
foreach(ITEM ${SOURCES})
    get_filename_component(FILE_DIR  ${ITEM} DIRECTORY)
    get_filename_component(FILE_NAME ${ITEM} NAME)

    set(ITEM_DIR "${FILE_DIR}")
    string(REPLACE "${CONTENT_SOURCE_DIR}" "" ITEM_DIR ${ITEM_DIR})

    set(ITEM_DIR "${BUILD_DIR}/Content/${ITEM_DIR}")
    string(REPLACE "//" "/" ITEM_DIR ${ITEM_DIR})

    file(MAKE_DIRECTORY "${ITEM_DIR}")

    set(ITEM_FILE "${ITEM_DIR}/${FILE_NAME}")
    string(REPLACE "//" "/" ITEM_FILE ${ITEM_FILE})

    add_custom_command(
        TARGET
            ${CONTENT_TARGET_NAME}
        POST_BUILD
        COMMAND
            ${CMAKE_COMMAND} -E copy
                ${ITEM}
                ${ITEM_FILE}
    )
endforeach(ITEM)
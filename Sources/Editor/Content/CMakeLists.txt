##############  Set source files  #######################
file(
    GLOB_RECURSE
    SOURCES

    "${CONTENT_SOURCE_DIR}/*.box"
)

##############  Copy files  #######################
foreach(ITEM ${SOURCES})
    get_filename_component(FILE_DIR  ${ITEM} DIRECTORY)
    get_filename_component(FILE_NAME ${ITEM} NAME)

    set(ITEM_DIR "${FILE_DIR}")
    string(REPLACE "${CONTENT_SOURCE_DIR}" "" ITEM_DIR ${ITEM_DIR})

    set(ITEM_DIR "${CONTENT_BUILD_DIR}/${ITEM_DIR}")
    string(REPLACE "//" "/" ITEM_DIR ${ITEM_DIR})

    file(MAKE_DIRECTORY "${ITEM_DIR}")

    set(ITEM_FILE "${ITEM_DIR}/${FILE_NAME}")
    string(REPLACE "//" "/" ITEM_FILE ${ITEM_FILE})

    configure_file(${ITEM} ${ITEM_FILE} COPYONLY)

    list(APPEND FILES ${ITEM_FILE})
endforeach(ITEM)

add_custom_target(
    ${CONTENT_TARGET_NAME}
    DEPENDS

    ${FILES}
)
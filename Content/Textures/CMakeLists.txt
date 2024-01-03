##############  Set values  #######################
set(TARGET_NAME "${TEXTURES_TARGET_NAME}")
set(SOURCES_DIR "${TEXTURES_SOURCE_DIR}")

set(BUILD_CONTENT_TEXTURES_DIR "${CONTENT_BUILD_DIR}/Textures")

##############  Set source files  #######################
file(
    GLOB_RECURSE
    SOURCES

    "${SOURCES_DIR}/*.png"
)

##############  Copy files  #######################
foreach(TEXTURE ${SOURCES})
    get_filename_component(FILE_DIR       ${TEXTURE} DIRECTORY)
    get_filename_component(FILE_NAME      ${TEXTURE} NAME)

    set(TEXTURE_DIR "${FILE_DIR}")
    string(REPLACE "${SOURCES_DIR}" "" TEXTURE_DIR ${TEXTURE_DIR})

    set(TEXTURE_DIR "${BUILD_CONTENT_TEXTURES_DIR}/${TEXTURE_DIR}")
    string(REPLACE "//" "/" TEXTURE_DIR ${TEXTURE_DIR})

    file(MAKE_DIRECTORY "${TEXTURE_DIR}")

    set(TEXTURE_FILE "${TEXTURE_DIR}/${FILE_NAME}")
    string(REPLACE "//" "/" TEXTURE_FILE ${TEXTURE_FILE})

    message("${TEXTURE_FILE}")

    configure_file(${TEXTURE} ${TEXTURE_FILE} COPYONLY)

    list(APPEND TEXTURE_FILES ${TEXTURE_FILE})
endforeach(TEXTURE)

add_custom_target(
    ${TARGET_NAME}
    DEPENDS

    ${TEXTURE_FILES}
)
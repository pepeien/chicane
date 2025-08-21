##############  Set target  #######################
add_custom_target(${EDITOR_CONTENT_TARGET_NAME})

##############  Copy files  #######################
CH_COPY_FILES(
    ${EDITOR_CONTENTS_TARGET_NAME}
    ${EDITOR_CONTENTS_SOURCE_DIR}
    ${BUILD_DIR}/Contents/Editor
    ".:*CMakeLists.txt|.frag|.vert$"
)

CH_COMPILE_SHADERS(
    ${EDITOR_CONTENTS_TARGET_NAME}
    ${EDITOR_CONTENTS_SOURCE_DIR}
    ${BUILD_DIR}/Contents/Editor
)
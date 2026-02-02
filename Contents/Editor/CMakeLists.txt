##############  Set target  #######################
add_custom_target(${EDITOR_CONTENT_TARGET_NAME})

##############  Set source files  #######################
CH_COPY_FILES(
    ${EDITOR_CONTENT_TARGET_NAME}
    ${EDITOR_CONTENT_SOURCE_DIR}
    ${BUILD_DIR}/Contents/Editor
    ".:*CMakeLists.txt|.frag|.vert$"
)

CH_COMPILE_SHADERS(
    ${EDITOR_CONTENT_TARGET_NAME}
    ${EDITOR_CONTENT_SOURCE_DIR}
    ${BUILD_DIR}/Contents/Editor
)
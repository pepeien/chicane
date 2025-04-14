##############  Set target  #######################
add_custom_target(${RUNTIME_CONTENT_TARGET_NAME})

##############  Copy files  #######################
CH_COPY_FILES(
    ${RUNTIME_CONTENT_TARGET_NAME}
    ${RUNTIME_CONTENT_SOURCE_DIR}
    ${BUILD_DIR}/Contents/Chicane/Runtime
    ".:*CMakeLists.txt|.frag|.vert$"
)

CH_COMPILE_VK_SHADERS(
    ${RUNTIME_CONTENT_TARGET_NAME}
    ${RUNTIME_CONTENT_SOURCE_DIR}
    ${BUILD_DIR}/Contents/Chicane/Runtime
)
##############  Set target  #######################
add_custom_target(${CONTENTS_TARGET_NAME})

##############  Copy files  #######################
CH_COPY_FILES(
    ${CONTENTS_TARGET_NAME}
    ${CONTENTS_SOURCE_DIR}
    ${BUILD_DIR}/Contents
    ".:*CMakeLists.txt|.frag|.vert$"
)

CH_COMPILE_SHADERS(
    ${CONTENTS_TARGET_NAME}
    ${CONTENTS_SOURCE_DIR}
    ${BUILD_DIR}/Contents
)
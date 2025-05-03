##############  Set target  #######################
add_custom_target(${CONTENTS_TARGET_NAME})

##############  Copy files  #######################
CH_COPY_FILES(
    ${CONTENTS_TARGET_NAME}
    ${CONTENTS_SOURCE_DIR}
    ${BUILD_DIR}/Engine
    ".:*CMakeLists.txt|.frag|.vert$"
)

CH_COMPILE_VK_SHADERS(
    ${CONTENTS_TARGET_NAME}
    ${CONTENTS_SOURCE_DIR}
    ${BUILD_DIR}/Engine
)
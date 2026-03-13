##################### Set target #####################
add_custom_target(${CHICANE_EDITOR_ASSETS_TARGET_NAME})

##################### Copy files #####################
CH_COPY_FILES(
    ${CHICANE_EDITOR_ASSETS_TARGET_NAME}
    ${CHICANE_EDITOR_ASSETS_SOURCES_DIR}
    ${CHICANE_EDITOR_ASSETS_BUILD_DIR}
    ".:*CMakeLists.txt|.frag|.vert$"
)

CH_COMPILE_SHADERS(
    ${CHICANE_EDITOR_ASSETS_TARGET_NAME}
    ${CHICANE_EDITOR_ASSETS_SOURCES_DIR}
    ${CHICANE_EDITOR_ASSETS_BUILD_DIR}
)
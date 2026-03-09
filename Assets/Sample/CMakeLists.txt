##################### Set target #####################
add_custom_target(${CHICANE_SAMPLE_ASSETS_TARGET_NAME})

##################### Copy files #####################
CH_COPY_FILES(
    ${CHICANE_SAMPLE_ASSETS_TARGET_NAME}
    ${CHICANE_SAMPLE_ASSETS_SOURCES_DIR}
    ${CHICANE_SAMPLE_ASSETS_BUILD_DIR}
    ".:*CMakeLists.txt|.frag|.vert$"
)

CH_COMPILE_SHADERS(
    ${CHICANE_SAMPLE_ASSETS_TARGET_NAME}
    ${CHICANE_SAMPLE_ASSETS_SOURCES_DIR}
    ${CHICANE_SAMPLE_ASSETS_BUILD_DIR}
)
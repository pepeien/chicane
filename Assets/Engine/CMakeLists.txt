##################### Set target #####################
add_custom_target(${CHICANE_ENGINE_ASSETS_TARGET_NAME})

##################### Copy files #####################
CH_COPY_FILES(
    ${CHICANE_ENGINE_ASSETS_TARGET_NAME}
    ${CHICANE_ENGINE_ASSETS_SOURCES_DIR}
    ${CHICANE_ENGINE_ASSETS_BUILD_DIR}
    ".:*CMakeLists.txt|.frag|.vert$"
)

CH_COMPILE_SHADERS(
    ${CHICANE_ENGINE_ASSETS_TARGET_NAME}
    ${CHICANE_ENGINE_ASSETS_SOURCES_DIR}
    ${CHICANE_ENGINE_ASSETS_BUILD_DIR}
)
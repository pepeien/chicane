##############  Set target  #######################
add_custom_target(${SHOOTER_CONTENT_TARGET_NAME})

##############  Set source files  #######################
CH_COPY_FILES(
    ${SHOOTER_CONTENT_TARGET_NAME}
    ${SHOOTER_CONTENT_SOURCE_DIR}
    ${BUILD_DIR}/Contents/Sample/Shooter
    ".:*CMakeLists.txt|.frag|.vert$"
)
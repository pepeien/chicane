add_custom_target(${ASSETS_TARGET_NAME})

##############  Set values  #######################
set(SHADERS_TARGET_NAME "${ASSETS_TARGET_NAME}_Shaders")
set(SHADERS_SOURCE_DIR  "${ASSETS_DIR}/Shaders")

##############  Set directories  #######################
add_subdirectory(Shaders)

##############  Set dependencies  #######################
add_dependencies(${ASSETS_TARGET_NAME} ${SHADERS_TARGET_NAME})
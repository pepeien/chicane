set(VULKAN_SHADERS_TARGET_NAME "${CONTENT_TARGET_NAME}_Vulkan")
set(VULKAN_SHADERS_SOURCE_DIR  "${CONTENT_SOURCE_DIR}/Engine/Vulkan/Shaders")

##############  Add subdirectories  #######################
add_subdirectory(Shaders)

##############  Add dependencies  #######################
add_dependencies(${CONTENT_TARGET_NAME} ${VULKAN_SHADERS_TARGET_NAME})
##############  Add subdirectories  #######################
add_subdirectory(Shaders)

##############  Add dependencies  #######################
add_dependencies(${RUNTIME_CONTENT_TARGET_NAME} ${RUNTIME_CONTENT_VULKAN_SHADERS_TARGET_NAME})
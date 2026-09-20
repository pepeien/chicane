##############  Add subdirectories  #######################
# Glad
if (CHICANE_OPENGL)
    add_subdirectory(Glad)
endif()

# Vulkan Memory Allocator
if (CHICANE_VULKAN)
    set(VMA_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
    set(VMA_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
    set(VMA_BUILD_DOCUMENTATION OFF CACHE BOOL "" FORCE)

    add_subdirectory(VulkanMemoryAllocator EXCLUDE_FROM_ALL)
endif()

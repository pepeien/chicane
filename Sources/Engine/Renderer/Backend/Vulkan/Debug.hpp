#pragma once

#include <vulkan/vulkan.hpp>

#include "Chicane/Renderer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        namespace VulkanDebug
        {
            CHICANE_RENDERER void initMessenger(
                vk::DebugUtilsMessengerEXT&      outDebugMessenger,
                const vk::Instance&              inInstance,
                const vk::DispatchLoaderDynamic& inDispatcher
            );

            CHICANE_RENDERER void destroyMessenger(
                vk::DebugUtilsMessengerEXT&      outDebugMessenger,
                const vk::Instance&              inInstance,
                const vk::DispatchLoaderDynamic& inDispatcher
            );
        }
    }
}
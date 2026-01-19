#pragma once

#include <vulkan/vulkan.hpp>

#include "Chicane/Renderer.hpp"

namespace Chicane
{
    namespace Renderer
    {
        namespace VulkanDebug
        {
            CHICANE_RENDERER vk::Bool32 callback(
                vk::DebugUtilsMessageSeverityFlagBitsEXT      inMessageSeverity,
                vk::DebugUtilsMessageTypeFlagsEXT             inMessageType,
                const vk::DebugUtilsMessengerCallbackDataEXT* inData,
                void*                                         outUserData
            );

            CHICANE_RENDERER void initMessenger(
                vk::DebugUtilsMessengerEXT&              outDebugMessenger,
                const vk::Instance&                      inInstance,
                const vk::detail::DispatchLoaderDynamic& inDispatcher
            );

            CHICANE_RENDERER void destroyMessenger(
                vk::DebugUtilsMessengerEXT&              outDebugMessenger,
                const vk::Instance&                      inInstance,
                const vk::detail::DispatchLoaderDynamic& inDispatcher
            );
        }
    }
}
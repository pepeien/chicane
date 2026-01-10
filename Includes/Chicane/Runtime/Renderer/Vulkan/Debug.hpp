#pragma once

#include "Chicane/Runtime.hpp"

#include <vulkan/vulkan.hpp>

namespace Chicane
{
    namespace Vulkan
    {
        namespace Debug
        {
            CHICANE_RUNTIME vk::Bool32 debugCallback(
                vk::DebugUtilsMessageSeverityFlagBitsEXT      inMessageSeverity,
                vk::DebugUtilsMessageTypeFlagsEXT             inMessageType,
                const vk::DebugUtilsMessengerCallbackDataEXT* inData,
                void*                                         outUserData
            );

            CHICANE_RUNTIME void initMessenger(
                vk::DebugUtilsMessengerEXT&              outDebugMessenger,
                const vk::Instance&                      inInstance,
                const vk::detail::DispatchLoaderDynamic& inDispatcher
            );

            CHICANE_RUNTIME void destroyMessenger(
                vk::DebugUtilsMessengerEXT&              outDebugMessenger,
                const vk::Instance&                      inInstance,
                const vk::detail::DispatchLoaderDynamic& inDispatcher
            );
        }
    }
}
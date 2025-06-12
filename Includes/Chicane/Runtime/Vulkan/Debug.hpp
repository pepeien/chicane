#pragma once

#include "Chicane/Runtime/Vulkan/Essential.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Debug
        {
            CHICANE_RUNTIME VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback(
                vk::DebugUtilsMessageSeverityFlagBitsEXT inMessageSeverity,
                vk::DebugUtilsMessageTypeFlagsEXT inMessageType,
                const vk::DebugUtilsMessengerCallbackDataEXT* inData,
                void* outUserData
            );

            CHICANE_RUNTIME void initMessenger(
                vk::DebugUtilsMessengerEXT& outDebugMessenger,
                const vk::Instance& inInstance,
                const vk::detail::DispatchLoaderDynamic& inDispatcher
            );

            CHICANE_RUNTIME void destroyMessenger(
                vk::DebugUtilsMessengerEXT& outDebugMessenger,
                const vk::Instance& inInstance,
                const vk::detail::DispatchLoaderDynamic& inDispatcher
            );
        }
    }
}
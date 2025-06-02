#pragma once

#include "Chicane/Runtime/Vulkan/Essential.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Debug
        {
            CHICANE_RUNTIME void initMessenger(
                vk::DebugUtilsMessengerEXT& outDebugMessenger,
                const vk::Instance& inInstance,
                const vk::DispatchLoaderDynamic& inDldi
            );
        
            CHICANE_RUNTIME VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
                VkDebugUtilsMessageSeverityFlagBitsEXT inMessageSeverity,
                VkDebugUtilsMessageTypeFlagsEXT inMessageType,
                const VkDebugUtilsMessengerCallbackDataEXT* inData,
                void* outUserData
            );
        }
    }
}
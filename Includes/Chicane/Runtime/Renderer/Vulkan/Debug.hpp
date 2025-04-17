#pragma once

#include "Runtime/Renderer/Vulkan/Base.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Debug
        {
            CHICANE void initMessenger(
                vk::DebugUtilsMessengerEXT& outDebugMessenger,
                const vk::Instance& inInstance,
                const vk::DispatchLoaderDynamic& inDldi
            );
        
            CHICANE VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
                VkDebugUtilsMessageSeverityFlagBitsEXT inMessageSeverity,
                VkDebugUtilsMessageTypeFlagsEXT inMessageType,
                const VkDebugUtilsMessengerCallbackDataEXT* inData,
                void* outUserData
            );
        }
    }
}
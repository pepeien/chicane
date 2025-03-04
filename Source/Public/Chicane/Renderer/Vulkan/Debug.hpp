#pragma once

#include "Chicane/Renderer/Vulkan/Base.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Debug
        {
            void initMessenger(
                vk::DebugUtilsMessengerEXT& outDebugMessenger,
                const vk::Instance& inInstance,
                const vk::DispatchLoaderDynamic& inDldi
            );
        
            VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
                VkDebugUtilsMessageSeverityFlagBitsEXT inMessageSeverity,
                VkDebugUtilsMessageTypeFlagsEXT inMessageType,
                const VkDebugUtilsMessengerCallbackDataEXT* inData,
                void* outUserData
            );
        }
    }
}
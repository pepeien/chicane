#pragma once

#include "Base.hpp"

namespace Engine
{
    namespace Runtime
    {
        namespace Renderer
        {
            namespace Debug
            {
                void initMessenger(
                    vk::DebugUtilsMessengerEXT& outDebugMessenger,
                    const vk::Instance& inInstance,
                    const vk::DispatchLoaderDynamic& inDldi
                );

                VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
                    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                    VkDebugUtilsMessageTypeFlagsEXT messageType,
                    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                    void* pUserData
                );
            }
        }
    }
}
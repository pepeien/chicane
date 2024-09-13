#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Log.hpp"

namespace Chicane
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
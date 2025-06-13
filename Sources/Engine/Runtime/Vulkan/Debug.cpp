#include "Chicane/Runtime/Vulkan/Debug.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Debug
        {
            vk::Bool32 debugCallback(
                vk::DebugUtilsMessageSeverityFlagBitsEXT inMessageSeverity,
                vk::DebugUtilsMessageTypeFlagsEXT inMessageType,
                const vk::DebugUtilsMessengerCallbackDataEXT* inData,
                void* outUserData
            )
            {
                if (!inData->pMessage)
                {
                    return VK_FALSE;
                }

                String prefix = "General";
                if (inMessageType == vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance)
                {
                    prefix = "Performance";
                }
                else if (inMessageType == vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation)
                {
                    prefix = "Validation";
                }
                else if (inMessageType == vk::DebugUtilsMessageTypeFlagBitsEXT::eDeviceAddressBinding)
                {
                    prefix = "Device";
                }

                String color = Color::HEX_COLOR_WHITE;
                if (inMessageSeverity == vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning)
                {
                    color = Color::HEX_COLOR_YELLOW;
                }
                else if (inMessageSeverity == vk::DebugUtilsMessageSeverityFlagBitsEXT::eError)
                {
                    color = Color::HEX_COLOR_ORANGE;
                }

                Log::emmit(color, "Vulkan", prefix + " | " + inData->pMessage);

                return VK_FALSE;
            }

            void initMessenger(
                vk::DebugUtilsMessengerEXT& outDebugMessenger,
                const vk::Instance& inInstance,
                const vk::detail::DispatchLoaderDynamic& inDispatcher
            )
            {
                if (!IS_DEBUGGING)
                {
                    return;
                }

                vk::DebugUtilsMessengerCreateInfoEXT createInfo = vk::DebugUtilsMessengerCreateInfoEXT(
                    vk::DebugUtilsMessengerCreateFlagsEXT(),

                    // Severity
                    vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
                    vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo    |
                    vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                    vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,

                    // Type
                    vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral     |
                    vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation  |
                    vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,

                    // Callback
                    debugCallback
                );

                outDebugMessenger = inInstance.createDebugUtilsMessengerEXT(createInfo, nullptr, inDispatcher);
            }

            void destroyMessenger(
                vk::DebugUtilsMessengerEXT& outDebugMessenger,
                const vk::Instance& inInstance,
                const vk::detail::DispatchLoaderDynamic& inDispatcher
            )
            {
                if (!IS_DEBUGGING)
                {
                    return;
                }

                inInstance.destroyDebugUtilsMessengerEXT(outDebugMessenger, nullptr, inDispatcher);
            }
        }
    }
}
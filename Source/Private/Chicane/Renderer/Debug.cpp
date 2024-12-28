#include "Chicane/Renderer/Debug.hpp"

#include "Chicane/Core/Log.hpp"

namespace Chicane
{
    namespace Debug
    {
        void initMessenger(
            vk::DebugUtilsMessengerEXT& outDebugMessenger,
            const vk::Instance& inInstance,
            const vk::DispatchLoaderDynamic& inDldi
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
                vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
                vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
                vk::DebugUtilsMessageTypeFlagBitsEXT::eDeviceAddressBinding,

                debugCallback,
                nullptr
            );

            outDebugMessenger = inInstance.createDebugUtilsMessengerEXT(
                createInfo,
                nullptr,
                inDldi
            );
        }

        VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT inSeverity,
            VkDebugUtilsMessageTypeFlagsEXT inMessageType,
            const VkDebugUtilsMessengerCallbackDataEXT* inData,
            void* outUserData
        )
        {
            if (!inData->pMessage)
            {
                return VK_FALSE;
            }

            std::string prefix = "Vk::";
            std::string color  = Chicane::Color::White;

            switch (inMessageType)
            {
            case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
                prefix += "Performance";

                break;

            case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
                prefix += "Validation";

                break;

            case VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT:
                prefix += "Device";

                break;

            default:
                prefix += "General";

                break;
            }

            prefix += " ";

            switch (inSeverity)
            {
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                color = Chicane::Color::Yellow;

                break;

            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                color = Chicane::Color::Orange;

                break;
            }

            Chicane::Log::emmit(
                color,
                "Vulkan",
                prefix + inData->pMessage
            );

            return VK_FALSE;
        }
    }
}
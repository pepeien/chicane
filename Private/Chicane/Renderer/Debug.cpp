#include "Chicane/Renderer/Debug.hpp"

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
            if (IS_DEBUGGING == false)
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
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* pUserData
        )
        {
            switch (messageType)
            {
            case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
                LOG_INFO(pCallbackData->pMessage);

                break;

            case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
                LOG_WARNING(pCallbackData->pMessage);

                break;

            case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
                LOG_ERROR(pCallbackData->pMessage);

                break;

            default:
                LOG_CRITICAL(pCallbackData->pMessage);

                break;
            }

            return VK_FALSE;
        }
    }
}
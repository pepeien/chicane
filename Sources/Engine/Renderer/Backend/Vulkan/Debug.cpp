#include "Chicane/Renderer/Backend/Vulkan/Debug.hpp"

#include "Chicane/Core/Color.hpp"
#include "Chicane/Core/Log.hpp"
#include "Chicane/Core/String.hpp"

namespace Chicane
{
    namespace Renderer
    {
        namespace VulkanDebug
        {
            static VKAPI_ATTR VkBool32 VKAPI_CALL callback(
                VkDebugUtilsMessageSeverityFlagBitsEXT      inMessageSeverity,
                VkDebugUtilsMessageTypeFlagsEXT             inMessageType,
                const VkDebugUtilsMessengerCallbackDataEXT* inData,
                void*                                       inUser
            )
            {
                if (!inData->pMessage)
                {
                    return VK_FALSE;
                }

                String prefix = "General";
                if (inMessageType ==
                    VkDebugUtilsMessageTypeFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT)
                {
                    prefix = "Performance";
                }
                else if (inMessageType ==
                         VkDebugUtilsMessageTypeFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT)
                {
                    prefix = "Validation";
                }
                else if (inMessageType ==
                         VkDebugUtilsMessageTypeFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT)
                {
                    prefix = "Device";
                }

                String color = Color::HEX_COLOR_WHITE;
                if (inMessageSeverity ==
                    VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
                {
                    color = Color::HEX_COLOR_YELLOW;
                }
                else if (inMessageSeverity ==
                         VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
                {
                    color = Color::HEX_COLOR_ORANGE;
                }

                Log::emmit(color, "Vulkan", prefix + " | " + inData->pMessage);

                return VK_FALSE;
            }

            void initMessenger(
                vk::DebugUtilsMessengerEXT&      outDebugMessenger,
                const vk::Instance&              inInstance,
                const vk::DispatchLoaderDynamic& inDispatcher
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
                        vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
                        vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                        vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,

                    // Type
                    vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
                        vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,

                    // Callback
                    callback
                );

                outDebugMessenger = inInstance.createDebugUtilsMessengerEXT(createInfo, nullptr, inDispatcher);
            }

            void destroyMessenger(
                vk::DebugUtilsMessengerEXT&      outDebugMessenger,
                const vk::Instance&              inInstance,
                const vk::DispatchLoaderDynamic& inDispatcher
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
#include "Backend/Vulkan/Debug.hpp"

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

                String     prefix                  = "General";
                const bool bMessageTypePerformance = static_cast<bool>(
                    inMessageType == VkDebugUtilsMessageTypeFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT
                );

                if (bMessageTypePerformance)
                {
                    prefix = "Performance";
                }

                const bool bMessageTypeValidation =
                    !bMessageTypePerformance &&
                    (inMessageType ==
                     VkDebugUtilsMessageTypeFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT);

                if (bMessageTypeValidation)
                {
                    prefix = "Validation";
                }

                const bool bMessageTypeDeviceAddressBinding =
                    !bMessageTypePerformance && !bMessageTypeValidation &&
                    (inMessageType ==
                     VkDebugUtilsMessageTypeFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT);

                if (bMessageTypeDeviceAddressBinding)
                {
                    prefix = "Device";
                }

                String     color                   = Color::HEX_COLOR_WHITE;
                const bool bMessageSeverityWarning = static_cast<bool>(
                    inMessageSeverity ==
                    VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
                );

                if (bMessageSeverityWarning)
                {
                    color = Color::HEX_COLOR_YELLOW;
                }

                const bool bMessageSeverityError =
                    !bMessageSeverityWarning &&
                    (inMessageSeverity ==
                     VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT);

                if (bMessageSeverityError)
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
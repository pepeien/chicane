#pragma once

#include <vulkan/vulkan.hpp>

#include "Chicane/Renderer.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Buffer.hpp"
#include "Chicane/Renderer/Backend/Vulkan/Buffer/CreateInfo.hpp"

namespace Chicane
{
    namespace Renderer
    {
        template <typename T>
        struct CHICANE_RENDERER VulkanFrameResource
        {
        public:
            void setup(const VulkanBufferCreateInfo& inCreateInfo)
            {
                m_frame.init(inCreateInfo);

                m_allocationSize  = inCreateInfo.size;
                m_writeLocation   = inCreateInfo.logicalDevice.mapMemory(m_frame.memory, 0, m_allocationSize);

                bufferInfo.buffer = m_frame.instance;
                bufferInfo.offset = 0;
                bufferInfo.range  = m_allocationSize;

                m_bIsDirty        = false;
            }

            void destroy(const vk::Device& inLogicalDevice)
            {
                m_bIsDirty = true;

                if (!m_frame.memory)
                {
                    return;
                }

                inLogicalDevice.unmapMemory(m_frame.memory);

                m_frame.destroy(inLogicalDevice);
            }

            bool isDirty() const { return m_bIsDirty; }

            void setAsDirty() { m_bIsDirty = true; }

            void copyToBuffer(const T* inData)
            {
                memcpy(m_writeLocation, inData, m_allocationSize);

                m_bIsDirty = false;
            }

        public:
            vk::DescriptorBufferInfo bufferInfo = {};

        private:
            // Status
            bool         m_bIsDirty       = true;

            // Memory
            size_t       m_allocationSize = 0;
            void*        m_writeLocation  = nullptr;
            VulkanBuffer m_frame          = {};
        };
    }
}
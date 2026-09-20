#pragma once

#include <cstddef>
#include <cstring>

#include <vulkan/vulkan.hpp>

#include "Chicane/Renderer.hpp"
#include "Backend/Vulkan/Buffer.hpp"
#include "Backend/Vulkan/Buffer/CreateInfo.hpp"

namespace Chicane
{
    namespace Renderer
    {
        template <typename T>
        struct VulkanFrameResource
        {
        public:
            void setup(const VulkanBufferCreateInfo& inCreateInfo)
            {
                m_frame.init(inCreateInfo);

                m_allocationSize = inCreateInfo.size;
                m_writeLocation  = m_frame.map();

                bufferInfo.buffer = m_frame.instance;
                bufferInfo.offset = 0;
                bufferInfo.range  = m_allocationSize;

                m_bIsDirty = false;
            }

            void destroy(const vk::Device&)
            {
                m_bIsDirty = true;

                if (isEmpty())
                {
                    return;
                }

                m_frame.destroy();
                m_writeLocation = nullptr;
            }

            bool isDirty() const { return m_bIsDirty; }

            void setAsDirty() { m_bIsDirty = true; }

            void copyToBuffer(const T* inData, std::size_t inSize)
            {
                if (!m_writeLocation || !inData || inSize == 0 || m_allocationSize == 0)
                {
                    return;
                }

                memcpy(m_writeLocation, inData, inSize < m_allocationSize ? inSize : m_allocationSize);

                m_bIsDirty = false;
            }

            bool isEmpty() { return !m_frame.instance; }

        public:
            vk::DescriptorBufferInfo bufferInfo = {};

        private:
            // Status
            bool         m_bIsDirty = true;

            // Memory
            std::size_t  m_allocationSize = 0;
            void*        m_writeLocation  = nullptr;
            VulkanBuffer m_frame          = {};
        };
    }
}
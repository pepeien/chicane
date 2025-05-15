#pragma once

#include "Chicane/Runtime/Renderer/Vulkan/Essential.hpp"
#include "Chicane/Runtime/Renderer/Vulkan/Buffer.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Frame
        {
            template<typename T>
            struct CHICANE_RUNTIME Resource
            {
            public:
                void setup(const Vulkan::Buffer::CreateInfo& inBufferCreateInfo)
                {
                    Vulkan::Buffer::init(m_buffer, inBufferCreateInfo);

                    m_allocationSize = inBufferCreateInfo.size;
                    m_writeLocation  = inBufferCreateInfo.logicalDevice.mapMemory(m_buffer.memory, 0, m_allocationSize);

                    bufferInfo.buffer = m_buffer.instance;
                    bufferInfo.offset = 0;
                    bufferInfo.range  = m_allocationSize;

                    m_isDirty = false;
                }

                void destroy(const vk::Device& inLogicalDevice)
                {
                    m_isDirty = true;

                    if (!m_buffer.memory)
                    {
                        return;
                    }

                    inLogicalDevice.unmapMemory(m_buffer.memory);

                    Vulkan::Buffer::destroy(inLogicalDevice, m_buffer);
                }

                bool isDirty() const
                {
                    return m_isDirty;
                }

                void setAsDirty()
                {
                    m_isDirty = true;
                }

                void copyToBuffer(const T* inData)
                {
                    memcpy(m_writeLocation, inData, m_allocationSize);

                    m_isDirty = false;
                }

            public: 
                vk::DescriptorBufferInfo bufferInfo       = {};

            private:
                // Status
                bool                     m_isDirty        = true;

                // Memory
                size_t                   m_allocationSize = 0;
                void*                    m_writeLocation  = nullptr;
                Vulkan::Buffer::Instance m_buffer         = {};
            };
        }
    }
}
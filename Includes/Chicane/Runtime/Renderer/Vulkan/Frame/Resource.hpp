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
                void setup(const Vulkan::BufferCreateInfo& inBufferCreateInfo)
                {
                    m_buffer.init(inBufferCreateInfo);

                    m_allocationSize = inBufferCreateInfo.size;
                    m_writeLocation  = inBufferCreateInfo.logicalDevice.mapMemory(m_buffer.memory, 0, m_allocationSize);

                    bufferInfo.buffer = m_buffer.instance;
                    bufferInfo.offset = 0;
                    bufferInfo.range  = m_allocationSize;

                    m_bIsDirty = false;
                }

                void destroy(const vk::Device& inLogicalDevice)
                {
                    m_bIsDirty = true;

                    if (!m_buffer.memory)
                    {
                        return;
                    }

                    inLogicalDevice.unmapMemory(m_buffer.memory);

                    m_buffer.destroy(inLogicalDevice);
                }

                bool isDirty() const
                {
                    return m_bIsDirty;
                }

                void setAsDirty()
                {
                    m_bIsDirty = true;
                }

                void copyToBuffer(const T* inData)
                {
                    memcpy(m_writeLocation, inData, m_allocationSize);

                    m_bIsDirty = false;
                }

            public: 
                vk::DescriptorBufferInfo bufferInfo       = {};

            private:
                // Status
                bool                     m_bIsDirty       = true;

                // Memory
                size_t                   m_allocationSize = 0;
                void*                    m_writeLocation  = nullptr;
                Vulkan::Buffer           m_buffer         = {};
            };
        }
    }
}
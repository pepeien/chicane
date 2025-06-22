#pragma once

#include "Chicane/Runtime/Backend/Vulkan/Essential.hpp"
#include "Chicane/Runtime/Backend/Vulkan/Image.hpp"
#include "Chicane/Runtime/Backend/Vulkan/Texture/CreateInfo.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Texture
        {
            class CHICANE_RUNTIME Instance
            {
            public:
                Instance(const CreateInfo& inCreateInfo);
                ~Instance();

            public:
                const Image::Data& getImage() const;

            private:
                void populate();

            private:
                Image::Data        m_image;

                vk::Device         m_logicalDevice;
                vk::PhysicalDevice m_physicalDevice;
                vk::CommandBuffer  m_commandBuffer;
                vk::Queue          m_queue;
            };
        }
    }
}
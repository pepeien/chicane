#pragma once

#include "Chicane/Renderer/Vulkan/Base.hpp"
#include "Chicane/Renderer/Vulkan/Image/Bundle.hpp"
#include "Chicane/Renderer/Vulkan/Texture/CreateInfo.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Texture
        {
            class Instance
            {
            public:
                Instance(const CreateInfo& inCreateInfo);
                ~Instance();

            public:
                const Image::Bundle& getImage() const;

            private:
                void load();
                void populate();

                void initView();
                void initSampler();

            private:
                int                        m_width;
                int                        m_height;
                int                        m_channels;
                std::vector<unsigned char> m_data;
                unsigned char*             m_pixels;

                Image::Bundle              m_image;

                vk::Device                 m_logicalDevice;
                vk::PhysicalDevice         m_physicalDevice;
                vk::CommandBuffer          m_commandBuffer;
                vk::Queue                  m_queue;
            };
        }
    }
}
#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Renderer/Image/Bundle.hpp"
#include "Chicane/Renderer/Texture/CreateInfo.hpp"

namespace Chicane
{
    namespace Texture
    {
        class Instance
        {
        public:
            Instance(const CreateInfo& inCreateInfo);
            ~Instance();

        public:
            Image::Bundle getImage() const;

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
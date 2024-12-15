#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Renderer/Buffer.hpp"
#include "Chicane/Renderer/Descriptor.hpp"
#include "Chicane/Renderer/Image.hpp"

constexpr uint32_t TEXTURE_IMAGE_COUNT = 1;

namespace Chicane
{
    namespace Texture
    {
        enum class Vendor : uint8_t
        {
            // N/A
            Undefined,
            // .png
            Png
        };

        struct CreateInfo
        {
            std::vector<unsigned char> data;
            vk::Device                 logicalDevice;
            vk::PhysicalDevice         physicalDevice;
            vk::CommandBuffer          commandBuffer;
            vk::Queue                  queue;
        };

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
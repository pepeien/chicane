#pragma once

#include "Base.hpp"

#include "Instance.hpp"

namespace Chicane
{
    namespace Texture
    {
        namespace Manager
        {
            class Instance
            {
            public:
                ~Instance();

            public:
                void addTexture(const std::string& inTextureId, const Texture::Data& inData);
                void bindTexture(
                    const std::string& inTextureId,
                    const vk::CommandBuffer& inCommandBuffer,
                    const vk::PipelineLayout& inPipelineLayout
                );

                void buildTexturesInstances(
                    const vk::Device& inLogicalDevice,
                    const vk::PhysicalDevice& inPhysicalDevice,
                    const vk::CommandBuffer& inCommandBuffer,
                    const vk::Queue& inQueue,
                    const vk::DescriptorSetLayout& inDescriptorSetLayout,
                    const vk::DescriptorPool& inDescriptorPool
                );
                void destroyTexturesInstances();

                uint32_t getCount();

            private:
                std::vector<std::string> m_registeredTextureIds;
                std::unordered_map<std::string, Texture::Data> m_textureDataMap;
                std::unordered_map<std::string, std::unique_ptr<Texture::Instance>> m_textureInstancesMap;
            };
        }
    }
}
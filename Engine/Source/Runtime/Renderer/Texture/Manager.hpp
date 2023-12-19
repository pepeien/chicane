#pragma once

#include "Base.hpp"

#include "Instance.hpp"

namespace Engine
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
                Texture::Instance* getTexture(const std::string& inTextureId); 

                void buildTextures(
                    const vk::Device& inLogicalDevice,
                    const vk::PhysicalDevice& inPhysicalDevice,
                    const vk::CommandBuffer& inCommandBuffer,
                    const vk::Queue& inQueue,
                    const vk::DescriptorSetLayout& inDescriptorSetLayout,
                    const vk::DescriptorPool& inDescriptorPool
                );

                uint32_t getCount();

            private:
                std::vector<std::string> registeredTextureIds;
                std::unordered_map<std::string, Texture::Data> textureDataMap;
                std::unordered_map<std::string, Texture::Instance*> textureInstancesMap;
            };
        }
    }
}
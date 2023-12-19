#include "Manager.hpp"

namespace Engine
{
    namespace Texture
    {
        namespace Manager
        {
            Instance::~Instance()
            {
                for (auto [id, texture] : textureInstancesMap)
                {
                    if (texture == nullptr)
                    {
                        continue;
                    }

                    delete texture;
                }
            }

            void Instance::addTexture(const std::string& inTextureId, const Texture::Data& inData)
            {
                if (
                    textureDataMap.find(inTextureId) != textureDataMap.end() ||
                    textureInstancesMap.find(inTextureId) != textureInstancesMap.end()
                )
                {
                    throw std::runtime_error("A Texture identified by [" + inTextureId  + "] already exists");
                }

                textureDataMap.insert(std::make_pair(inTextureId, inData));
                registeredTextureIds.push_back(inTextureId);
            }

            Texture::Instance* Instance::getTexture(const std::string& inTextureId)
            {
                auto foundTexture = textureInstancesMap.find(inTextureId);

                if (foundTexture == textureInstancesMap.end())
                {
                    throw std::runtime_error("Invalid Texture ID [" + inTextureId + "]");
                }

                return foundTexture->second;
            }

            void Instance::buildTextures(
                const vk::Device& inLogicalDevice,
                const vk::PhysicalDevice& inPhysicalDevice,
                const vk::CommandBuffer& inCommandBuffer,
                const vk::Queue& inQueue,
                const vk::DescriptorSetLayout& inDescriptorSetLayout,
                const vk::DescriptorPool& inDescriptorPool
            )
            {
                Texture::CreateInfo textureCreateInfo;
                textureCreateInfo.logicalDevice       = inLogicalDevice;
                textureCreateInfo.physicalDevice      = inPhysicalDevice;
                textureCreateInfo.commandBuffer       = inCommandBuffer;
                textureCreateInfo.queue               = inQueue;
                textureCreateInfo.descriptorSetLayout = inDescriptorSetLayout;
                textureCreateInfo.descriptorPool      = inDescriptorPool;

                for (std::string& textureId : registeredTextureIds)
                {
                    auto foundTexture = textureDataMap.find(textureId);

                    if (foundTexture == textureDataMap.end())
                    {
                        throw std::runtime_error("Invalid Texture ID [" + textureId + "]");
                    }

                    textureCreateInfo.data = foundTexture->second;

                    textureInstancesMap.insert(std::make_pair(textureId, new Texture::Instance(textureCreateInfo)));
                }
            }

            uint32_t Instance::getCount()
            {
                return textureDataMap.size();
            }
        }
    }
}
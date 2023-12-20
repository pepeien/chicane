#include "Manager.hpp"

namespace Engine
{
    namespace Texture
    {
        namespace Manager
        {
            Instance::~Instance()
            {
                destroyTexturesInstances();
            }

            void Instance::addTexture(const std::string& inTextureId, const Texture::Data& inData)
            {
                if (
                    textureDataMap.find(inTextureId) != textureDataMap.end() ||
                    textureInstancesMap.find(inTextureId) != textureInstancesMap.end()
                )
                {
                    throw std::runtime_error(
                        "The Texture [" + inTextureId  + "] has already been intiialized"
                    );
                }

                textureDataMap.insert(std::make_pair(inTextureId, inData));
                registeredTextureIds.push_back(inTextureId);
            }

            void Instance::bindTexture(
                const std::string& inTextureId,
                const vk::CommandBuffer& inCommandBuffer,
                const vk::PipelineLayout& inPipelineLayout
            )
            {
                auto foundTexture = textureInstancesMap.find(inTextureId);

                if (foundTexture == textureInstancesMap.end())
                {
                    throw std::runtime_error(
                        "The Texture [" + inTextureId + "] does not exists"
                    );
                }

                if (foundTexture->second == nullptr)
                {
                    throw std::runtime_error(
                        "The Texture [" + inTextureId + "] has not been initialized"
                    );
                }

                foundTexture->second->bind(inCommandBuffer, inPipelineLayout);
            }

            void Instance::buildTexturesInstances(
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
                        throw std::runtime_error("The Texture [" + textureId + "] does not exist");
                    }

                    textureCreateInfo.data = foundTexture->second;

                    textureInstancesMap.insert(
                        std::make_pair(
                            textureId,
                            std::make_unique<Texture::Instance>(textureCreateInfo)
                        )
                    );
                }
            }

            void Instance::destroyTexturesInstances()
            {
                textureInstancesMap.clear();
            }

            uint32_t Instance::getCount()
            {
                return textureDataMap.size();
            }
        }
    }
}
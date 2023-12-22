#include "Manager.hpp"

namespace Chicane
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
                    m_textureDataMap.find(inTextureId) != m_textureDataMap.end() ||
                    m_textureInstancesMap.find(inTextureId) != m_textureInstancesMap.end()
                )
                {
                    throw std::runtime_error(
                        "The Texture [" + inTextureId  + "] has already been intiialized"
                    );
                }

                m_textureDataMap.insert(std::make_pair(inTextureId, inData));
                m_registeredTextureIds.push_back(inTextureId);
            }

            void Instance::bindTexture(
                const std::string& inTextureId,
                const vk::CommandBuffer& inCommandBuffer,
                const vk::PipelineLayout& inPipelineLayout
            )
            {
                auto foundTexture = m_textureInstancesMap.find(inTextureId);

                if (foundTexture == m_textureInstancesMap.end())
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

                for (std::string& textureId : m_registeredTextureIds)
                {
                    auto foundTexture = m_textureDataMap.find(textureId);

                    if (foundTexture == m_textureDataMap.end())
                    {
                        throw std::runtime_error("The Texture [" + textureId + "] does not exist");
                    }

                    textureCreateInfo.data = foundTexture->second;

                    m_textureInstancesMap.insert(
                        std::make_pair(
                            textureId,
                            std::make_unique<Texture::Instance>(textureCreateInfo)
                        )
                    );
                }
            }

            void Instance::destroyTexturesInstances()
            {
                m_textureInstancesMap.clear();
            }

            uint32_t Instance::getCount()
            {
                return m_textureDataMap.size();
            }
        }
    }
}
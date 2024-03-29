#include "Runtime/Renderer/Texture/Manager.hpp"

namespace Chicane
{
    namespace Texture
    {
        namespace Manager
        {
            Instance::~Instance()
            {
                m_instanceMap.clear();
            }

            uint32_t Instance::getCount()
            {
                return static_cast<uint32_t>(m_dataMap.size());
            }

            void Instance::add(
                const std::string& inId,
                const std::vector<unsigned char>& inData
            )
            {
                if (
                    m_dataMap.find(inId) != m_dataMap.end() ||
                    m_instanceMap.find(inId) != m_instanceMap.end()
                )
                {
                    LOG_WARNING("The texture [" + inId  + "] has already been added");

                    return;
                }

                m_dataMap.insert(std::make_pair(inId, inData));
                m_registeredIds.push_back(inId);
            }

            void Instance::bindAll(
                const vk::CommandBuffer& inCommandBuffer,
                const vk::PipelineLayout& inPipelineLayout
            )
            {
                for (std::string& id : m_registeredIds)
                {
                    bind(id, inCommandBuffer, inPipelineLayout);
                }
            }

            void Instance::build(
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

                for (std::string& id : m_registeredIds)
                {
                    auto foundPair = m_dataMap.find(id);

                    if (foundPair == m_dataMap.end())
                    {
                        throw std::runtime_error("The Texture [" + id + "] does not exist");
                    }

                    textureCreateInfo.data = foundPair->second;

                    m_instanceMap.insert(
                        std::make_pair(
                            id,
                            std::make_unique<Texture::Instance>(textureCreateInfo)
                        )
                    );
                }
            }

            void Instance::bind(
                const std::string& inId,
                const vk::CommandBuffer& inCommandBuffer,
                const vk::PipelineLayout& inPipelineLayout
            )
            {
                std::string standarizedId = inId.size() > 0 ? inId : "missing";

                auto foundPair = m_instanceMap.find(standarizedId);

                if (foundPair == m_instanceMap.end())
                {
                    throw std::runtime_error(
                        "The Texture [" + standarizedId + "] does not exists"
                    );
                }

                if (foundPair->second == nullptr)
                {
                    throw std::runtime_error(
                        "The Texture [" + standarizedId + "] has not been initialized"
                    );
                }

                foundPair->second->bind(inCommandBuffer, inPipelineLayout);
            }
        }
    }
}
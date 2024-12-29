#include "Chicane/Renderer/CubeMap/Manager.hpp"

#include "Chicane/Renderer/CubeMap/CreateInfo.hpp"

namespace Chicane
{
    namespace CubeMap
    {
        Manager::~Manager()
        {
            m_instanceMap.clear();
        }

        std::uint32_t Manager::getCount() const
        {
            return static_cast<std::uint32_t>(m_dataMap.size());
        }

        void Manager::add(const std::string& inId, const CubeMap::Data& inData)
        {
            if (
                m_dataMap.find(inId) != m_dataMap.end() ||
                m_instanceMap.find(inId) != m_instanceMap.end()
            )
            {
                throw std::runtime_error(
                    "The Cube Map [" + inId  + "] has already been intiialized"
                );
            }

            m_dataMap.insert(std::make_pair(inId, inData));
            m_registeredIds.push_back(inId);
        }

        void Manager::bind(
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
                    "The Cube Map [" + standarizedId + "] does not exists"
                );
            }

            if (foundPair->second == nullptr)
            {
                throw std::runtime_error(
                    "The Cube Map [" + standarizedId + "] has not been initialized"
                );
            }

            foundPair->second->bind(inCommandBuffer, inPipelineLayout);
        }

        void Manager::build(
            const vk::Device& inLogicalDevice,
            const vk::PhysicalDevice& inPhysicalDevice,
            const vk::CommandBuffer& inCommandBuffer,
            const vk::Queue& inQueue,
            const vk::DescriptorSetLayout& inDescriptorSetLayout,
            const vk::DescriptorPool& inDescriptorPool
        )
        {
            CubeMap::CreateInfo createInfo;
            createInfo.logicalDevice       = inLogicalDevice;
            createInfo.physicalDevice      = inPhysicalDevice;
            createInfo.commandBuffer       = inCommandBuffer;
            createInfo.queue               = inQueue;
            createInfo.descriptorSetLayout = inDescriptorSetLayout;
            createInfo.descriptorPool      = inDescriptorPool;

            for (std::string& id : m_registeredIds)
            {
                auto foundPair = m_dataMap.find(id);

                if (foundPair == m_dataMap.end())
                {
                    throw std::runtime_error(
                        "The Cube Map [" + id + "] does not exist"
                    );
                }

                createInfo.data = foundPair->second;

                m_instanceMap.insert(
                    std::make_pair(
                        id,
                        std::make_unique<CubeMap::Instance>(createInfo)
                    )
                );
            }
        }

        void Manager::draw(
            const std::string& inId,
            const vk::CommandBuffer& inCommandBuffer
        )
        {
            auto foundModel = m_dataMap.find(inId);

            if (foundModel == m_dataMap.end())
            {
                throw std::runtime_error("The Cube Map [" + inId + "] does not exist");
            }

            inCommandBuffer.draw(
                CUBEMAP_IMAGE_COUNT,
                1,
                0,
                0
            );
        }
    }
}
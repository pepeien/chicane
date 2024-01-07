#pragma once

#include "Base.hpp"

#include "Instance.hpp"

namespace Chicane
{
    namespace CubeMap
    {
        namespace Manager
        {
            class Instance
            {
            public:
                ~Instance();

            public:
                uint32_t getCount();

                void add(const std::string& inId, const CubeMap::Data& inData);
                void bind(
                    const std::string& inId,
                    const vk::CommandBuffer& inCommandBuffer,
                    const vk::PipelineLayout& inPipelineLayout
                );
                void build(
                    const vk::Device& inLogicalDevice,
                    const vk::PhysicalDevice& inPhysicalDevice,
                    const vk::CommandBuffer& inCommandBuffer,
                    const vk::Queue& inQueue,
                    const vk::DescriptorSetLayout& inDescriptorSetLayout,
                    const vk::DescriptorPool& inDescriptorPool
                );
                void draw(
                    const std::string& inId,
                    const vk::CommandBuffer& inCommadBuffer
                );

            private:
                std::vector<std::string> m_registeredIds;

                std::unordered_map<std::string, CubeMap::Data> m_dataMap;
                std::unordered_map<std::string, std::unique_ptr<CubeMap::Instance>> m_instanceMap;
            };
        }
    }
}
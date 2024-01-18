#pragma once

#include "Base.hpp"

#include "Instance.hpp"

namespace Engine
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

        namespace Manager
        {
            class Instance
            {
            public:
                ~Instance();

            public:
                uint32_t getCount();

                void add(
                    const std::string& inId,
                    const std::vector<unsigned char>& inData
                );
                void bindAll(
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

            private:
                void bind(
                    const std::string& inId,
                    const vk::CommandBuffer& inCommandBuffer,
                    const vk::PipelineLayout& inPipelineLayout
                );

            private:
                std::vector<std::string> m_registeredIds;

                std::unordered_map<std::string, std::vector<unsigned char>> m_dataMap;
                std::unordered_map<std::string, std::unique_ptr<Texture::Instance>> m_instanceMap;
            };
        }
    }
}
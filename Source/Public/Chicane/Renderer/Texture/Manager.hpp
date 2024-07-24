#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Box.hpp"
#include "Chicane/Renderer/Texture.hpp"

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

        class Manager
        {
        public:
            ~Manager();

        public:
            uint32_t getCount();

            bool contains(const std::string& inId);
            void add(const std::string& inId, const Box::Entry& inEntry);
            void bindAll(
                const vk::CommandBuffer& inCommandBuffer,
                const vk::PipelineLayout& inPipelineLayout
            );
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

        private:
            std::vector<std::string> m_registeredIds;

            std::unordered_map<std::string, std::vector<unsigned char>> m_dataMap;
            std::unordered_map<std::string, std::unique_ptr<Texture::Instance>> m_instanceMap;
        };
    }
}
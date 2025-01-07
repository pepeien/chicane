#pragma once

#include "Chicane/Box/Asset/Texture.hpp"
#include "Chicane/Renderer/Manager.hpp"
#include "Chicane/Renderer/Vulkan/Base.hpp"
#include "Chicane/Renderer/Vulkan/Descriptor.hpp"
#include "Chicane/Renderer/Vulkan/Texture/Instance.hpp"

namespace Chicane
{
    namespace Vulkan
    {
        namespace Texture
        {
            class Manager : public Chicane::Manager<>
            {
            public:
                Manager();

            public:
                bool canDraw() const;
                bool canBind() const;
                bool contains(const std::string& inId) const;

                std::uint32_t getCount() const;
                std::uint32_t getIndex(const std::string& inId) const;

                void add(const std::string& inId, const Box::Texture* inAsset);
                void bindAll(
                    const vk::CommandBuffer& inCommandBuffer,
                    const vk::PipelineLayout& inPipelineLayout,
                    Descriptor::Bundle& inDescriptor
                );
                void bind(
                    const std::string& inId,
                    const vk::CommandBuffer& inCommandBuffer,
                    const vk::PipelineLayout& inPipelineLayout,
                    Descriptor::Bundle& inDescriptor
                );
                void build(
                    const vk::Device& inLogicalDevice,
                    const vk::PhysicalDevice& inPhysicalDevice,
                    const vk::CommandBuffer& inCommandBuffer,
                    const vk::Queue& inQueue,
                    Descriptor::Bundle& inDescriptor
                );

            private:
                std::vector<std::string> m_registeredIds;

                std::unordered_map<std::string, std::vector<unsigned char>> m_dataMap;
                std::unordered_map<std::string, std::unique_ptr<Texture::Instance>> m_instanceMap;
            };
        }
    }
}
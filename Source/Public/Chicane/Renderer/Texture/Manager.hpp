#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Box.hpp"
#include "Chicane/Core/Event.hpp"
#include "Chicane/Renderer/Texture.hpp"

namespace Chicane
{
    namespace Texture
    {
        class Manager
        {
        public:
            Manager();

        public:
            bool isEmpty() const;
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

            void watchChanges(
                std::function<void (void*)> inNextCallback,
                std::function<void (const std::string&)> inErrorCallback = nullptr,
                std::function<void ()> inCompleteCallback = nullptr
            );

        private:
            std::vector<std::string> m_registeredIds;

            std::unordered_map<std::string, std::vector<unsigned char>> m_dataMap;
            std::unordered_map<std::string, std::unique_ptr<Texture::Instance>> m_instanceMap;

            std::unique_ptr<Observable<void*>> m_observable;
        };
    }
}
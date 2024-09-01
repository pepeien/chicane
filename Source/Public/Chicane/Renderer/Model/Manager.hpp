#pragma once

#include "Chicane/Base.hpp"
#include "Chicane/Core/Box.hpp"
#include "Chicane/Renderer/Buffer.hpp"
#include "Chicane/Renderer/Model.hpp"
#include "Chicane/Renderer/Vertex.hpp"
#include "Chicane/Renderer/Model/Vendors/Wavefront.hpp"

namespace Chicane
{
    class MeshComponent;

    namespace Model
    {
        enum class Vendor : std::uint8_t
        {
            // N/A
            Undefined,
            // .obj
            Wavefront
        };

        class Manager
        {
            public:
                bool isLoaded(const std::string& inId) const;
                void load(const std::string& inId, const Box::Entry& inEntry);
                void use(const std::string& inId);
                void unUse(const std::string& inId);
                void build(
                    Buffer::Instance& outVertexBuffer,
                    Buffer::Instance& outIndexBuffer,
                    const vk::Device& inLogicalDevice,
                    const vk::PhysicalDevice& inPhysicalDevice,
                    const vk::Queue& inQueue,
                    const vk::CommandBuffer& inCommandBuffer
                );
                std::uint32_t getFirstInstance(const std::string& inId) const;
                void draw(
                    const std::string& inId,
                    const vk::CommandBuffer& inCommandBuffer
                );
                void drawAll(const vk::CommandBuffer& inCommandBuffer);

            private:
                void process(const std::string& inId);

                void initVertexBuffer(
                    Buffer::Instance& outVertexBuffer,
                    const vk::Device& inLogicalDevice,
                    const vk::PhysicalDevice& inPhysicalDevice,
                    const vk::Queue& inQueue,
                    const vk::CommandBuffer& inCommandBuffer
                );
                void initIndexBuffer(
                    Buffer::Instance& outIndexBuffer,
                    const vk::Device& inLogicalDevice,
                    const vk::PhysicalDevice& inPhysicalDevice,
                    const vk::Queue& inQueue,
                    const vk::CommandBuffer& inCommandBuffer
                );

            private:
                std::unordered_map<std::string, Model::AllocationInfo> m_allocationMap;
                std::unordered_map<std::string, Model::Instance> m_instanceMap;
                std::vector<std::string> m_usedIds;

                std::vector<Vertex::Instance> m_combinedVertices;
                std::vector<uint32_t> m_indexedVertices;
        };
    }
}
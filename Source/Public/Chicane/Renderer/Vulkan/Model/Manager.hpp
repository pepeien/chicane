#pragma once

#include "Chicane/Box/Asset/Model.hpp"
#include "Chicane/Renderer/Manager.hpp"
#include "Chicane/Renderer/Model.hpp"
#include "Chicane/Renderer/Model/Vendor/Wavefront.hpp"
#include "Chicane/Renderer/Vertex.hpp"
#include "Chicane/Renderer/Vulkan/Base.hpp"
#include "Chicane/Renderer/Vulkan/Buffer.hpp"

namespace Chicane
{
    class MeshComponent;

    namespace Vulkan
    {
        namespace Model
        {
            class Manager : public Chicane::Manager
            {
            public:
                Manager();

            public:
                bool isEmpty() const;
                bool canDraw() const;
                bool isLoaded(const std::string& inId) const;
                bool isAllocated(const std::string& inId) const;
                bool isUsing(const std::string& inId) const;

                const Vec<3, float>& getBounds(const std::string& inId);

                void load(const std::string& inId, const Box::Model* inAsset);
                void activate(const std::string& inId);
                void deactivate(const std::string& inId);
                void build(
                    Buffer::Instance& outVertexBuffer,
                    Buffer::Instance& outIndexBuffer,
                    const vk::Device& inLogicalDevice,
                    const vk::PhysicalDevice& inPhysicalDevice,
                    const vk::Queue& inQueue,
                    const vk::CommandBuffer& inCommandBuffer
                );

                std::uint32_t getUseCount(const std::string& inId) const;
                std::uint32_t getFirstInstance(const std::string& inId) const;
                void draw(
                    const std::string& inId,
                    const vk::CommandBuffer& inCommandBuffer
                );
                void drawAll(const vk::CommandBuffer& inCommandBuffer);

            private:
                void allocate(const std::string& inId);
                void deallocate(const std::string& inId);
                void rebaseAllocation();

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

                Vec<3, float> getExtent(const std::vector<Chicane::Vertex::Instance>& inVertices);

            private:
                std::unordered_map<std::string, Chicane::Model::Instance>       m_instanceMap;
                std::unordered_map<std::string, Chicane::Model::AllocationInfo> m_allocationMap;
                std::vector<std::string>                                        m_usedIds;

                std::vector<Chicane::Vertex::Instance>                          m_vertices;
                std::vector<std::uint32_t>                                      m_indices;
            };
        }
    }
}
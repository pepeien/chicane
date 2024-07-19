#pragma once

#include "Runtime/Runtime.hpp"
#include "Runtime/Renderer/Buffer.hpp"
#include "Runtime/Renderer/Model.hpp"
#include "Runtime/Renderer/Vertex.hpp"
#include "Runtime/Renderer/Model/Vendors/Wavefront.hpp"

namespace Chicane
{
    namespace Model
    {
        enum class Vendor : uint8_t
        {
            // N/A
            Undefined,
            // .obj
            Wavefront
        };

        namespace Manager
        {
            class Instance
            {
            public:
                void add(
                    const std::string& inId,
                    const std::vector<unsigned char>& inData,
                    Vendor inVendor = Vendor::Undefined
                );
                void build(
                    Buffer::Instance& outVertexBuffer,
                    Buffer::Instance& outIndexBuffer,
                    const vk::Device& inLogicalDevice,
                    const vk::PhysicalDevice& inPhysicalDevice,
                    const vk::Queue& inQueue,
                    const vk::CommandBuffer& inCommandBuffer
                );
                void drawAll(const vk::CommandBuffer& inCommandBuffer);

            private:
                void process(const std::string& inId);
                void processAll();

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

                void draw(
                    const std::string& inId,
                    const vk::CommandBuffer& inCommandBuffer,
                    uint32_t inFirstInstance
                );

            private:
                std::vector<Vertex::Instance> m_combinedVertices;
                std::vector<uint32_t> m_indexedVertices;

                std::vector<std::string> m_uniqueIds;
                std::vector<std::string> m_usedIds;

                std::unordered_map<std::string, Model::AllocationInfo> m_allocationMap;
                std::unordered_map<std::string, Model::Instance> m_instanceMap;
            };
        }
    }
}
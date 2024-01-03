#pragma once

#include "Base.hpp"

#include "Instance.hpp"
#include "Renderer/Buffer.hpp"
#include "Renderer/Vertex.hpp"
#include "Vendors/Wavefront.hpp"

namespace Chicane
{
    namespace Model
    {
        enum class Vendor
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
                std::vector<std::string> getModelIds();

                void addModel(
                    const std::string& inId,
                    const std::vector<Vertex::Instance>& inVertices,
                    const std::vector<uint32_t>& inIndexes
                );
                void importModel(
                    const std::string& inId,
                    const std::string& inFilePath,
                    Vendor inVendor
                );
                void drawModel(
                    const std::string& inId,
                    const vk::CommandBuffer& inCommadBuffer,
                    uint32_t inInstanceCount,
                    uint32_t inFirstInstance
                );
                void loadModels(
                    Buffer::Instance& outVertexBuffer,
                    Buffer::Instance& outIndexBuffer,
                    const vk::Device& inLogicalDevice,
                    const vk::PhysicalDevice& inPhysicalDevice,
                    const vk::Queue& inQueue,
                    const vk::CommandBuffer& inCommandBuffer
                );

            private:
                void processModels();

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
                std::vector<Vertex::Instance> m_combinedVertices;
                std::vector<uint32_t> m_indexedVertices;

                std::vector<std::string> m_registeredModelIds;
                std::unordered_map<std::string, Model::AllocationInfo> m_ModelAllocationInfos;
                std::unordered_map<std::string, Model::Instance> m_ModelInstances;
            };
        }
    }
}